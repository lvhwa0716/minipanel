#include <linux/spi/spi.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <mt_spi.h>
#include <mt_spi_hal.h>
#include <linux/miscdevice.h>

#include <linux/gpio.h>
#include "mt-plat/mt_gpio.h" // just op directly not use gpiolib, kkkk



#include "OledMono.h"

extern void Oled_Init(void);
extern void Oled_DeInit(void);

extern int oled128x32_init_status;
extern struct oled128x32_obj_t *oled128x32_ptr;
extern const struct file_operations oled128x32_fops;
extern int oled128x32_create_attr(struct device *dev);
extern int oled128x32_delete_attr(struct device *dev);


static struct spi_device *spi_oled128x32 = NULL;

#define SPI_FIFO_SIZE 32


static void drv_udelay(int usec)
{
	udelay(usec);
}


int oled_spi_write(const char *bufRaw, size_t count);


void Reset_Command(void)
{
	OledDriver_SetPin(GPIO_LCD_RST_IDX , LOW_LEVEL); //RES=0;
	mdelay(300);
	OledDriver_SetPin(GPIO_LCD_RST_IDX , HIGH_LEVEL);//RES=1;
	mdelay(300);
}
void Write_Command(unsigned char Data)
{
	unsigned char pBuf[2];
	pBuf[0] = Data;
	OledDriver_SetPin(GPIO_LCD_DC_IDX , LOW_LEVEL); //DC=1;
	drv_udelay(1);
	oled_spi_write(pBuf, 1);
}
void Write_Data(unsigned char Data)
{
	unsigned char pBuf[2];
	pBuf[0] = Data;

	OledDriver_SetPin(GPIO_LCD_DC_IDX , HIGH_LEVEL); //DC=1;
	drv_udelay(1);
	oled_spi_write(pBuf, 1);

}

void WriteMultiData(unsigned char *pBuf, int len)
{
	int j ;
	int sent_size = 0;
	OledDriver_SetPin(GPIO_LCD_DC_IDX , HIGH_LEVEL); //DC=1;
	drv_udelay(1);
	for(j=0;j< (len / SPI_FIFO_SIZE) ;j++)
	{
		oled_spi_write(pBuf, SPI_FIFO_SIZE);
		pBuf = pBuf + SPI_FIFO_SIZE;
		sent_size =  sent_size + SPI_FIFO_SIZE;
	}
	if( len > sent_size)
	{
		oled_spi_write(pBuf, len - sent_size);
	}

}
int oled_spi_write(const char *bufRaw, size_t count) {
	int ret = -EINVAL;
	struct spi_device *spi;
	struct spi_transfer transfer;
	struct spi_message msg;

	OLED_LOG("OLED_CONTROL count=%d\n",count);
	if(count <= 0) {
		OLED_LOG("OLED_CONTROL count error , ignore it\n");
		return 0;
	}

	memset(&transfer, 0 , sizeof(transfer));
	memset(&msg,0,sizeof(msg));

	spi = spi_oled128x32;

	if (unlikely(!spi)) {
		OLED_LOG("OLED_CONTROL spi device is invalid\n");
		goto out;
	}
	if (unlikely(!bufRaw)) {
		OLED_LOG("OLED_CONTROL buf is invalid\n");
		goto out;
	}
	/*
	if (unlikely((count > (16 * 32 * 2)))) {
		OLED_LOG("OLED_CONTROL buf too long , all hex must UPCASE\n");
		goto out;
	}
	*/
	spi_message_init(&msg);

	transfer.bits_per_word = 8;
	transfer.len = count;
	transfer.tx_buf = kmalloc(transfer.len, GFP_KERNEL);
	transfer.rx_buf = NULL;


	if (transfer.tx_buf == NULL)
		return -ENOMEM;


	#if (1) // debug
		if(transfer.len > 4)
		{
			OLED_LOG("OLED_CONTROL %02X%02X%02X%02X\n", bufRaw[0],bufRaw[1],bufRaw[2],bufRaw[3]);
		}
	#endif
	spi_message_add_tail(&transfer, &msg);
	ret = spi_sync(spi, &msg);

	kfree(transfer.tx_buf);

	if (ret < 0) {
		OLED_LOG("OLED_CONTROL Message transfer err:%d\n", ret);
	} else {
		ret = count;
	}

out:
	return ret;
}

ssize_t __spi_oled128x32_storeCfg(struct device *dev,
			 struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_device *spi;

	struct mt_chip_conf *chip_config;

	u32 setuptime, holdtime, high_time, low_time;
	u32 cs_idletime, ulthgh_thrsh;
	int cpol, cpha, tx_mlsb, rx_mlsb, tx_endian, sample_sel, cs_pol;
	int rx_endian, com_mod, pause, finish_intr;
	int deassert, tckdly, ulthigh;

	spi = container_of(dev, struct spi_device, dev);

	OLED_PRINT("SPIDEV name is:%s\n", spi->modalias);

	chip_config = (struct mt_chip_conf *)spi->controller_data;

	if (!chip_config) {
		OLED_PRINT("chip_config is NULL.\n");
		chip_config = kzalloc(sizeof(struct mt_chip_conf), GFP_KERNEL);
		if (!chip_config)
			return -ENOMEM;
	}

	if (!strncmp(buf, "-h", 2)) {
		OLED_PRINT("Please input the parameters for this device.\n");
	} else if (!strncmp(buf, "-w", 2)) {
		buf += 3;
		if (!buf) {
			OLED_LOG("buf is NULL.\n");
			goto out;
		}
		if (!strncmp(buf, "setuptime=", 10) && (1 == sscanf(buf + 10, "%d", &setuptime))) {
			OLED_PRINT("setuptime is:%d\n", setuptime);
			chip_config->setuptime = setuptime;
		} else if (!strncmp(buf, "holdtime=", 9) && (1 == sscanf(buf + 9, "%d", &holdtime))) {
			OLED_PRINT("Set holdtime is:%d\n", holdtime);
			chip_config->holdtime = holdtime;
		} else if (!strncmp(buf, "high_time=", 10)
			   && (1 == sscanf(buf + 10, "%d", &high_time))) {
			OLED_PRINT("Set high_time is:%d\n", high_time);
			chip_config->high_time = high_time;
		} else if (!strncmp(buf, "low_time=", 9) && (1 == sscanf(buf + 9, "%d", &low_time))) {
			OLED_PRINT("Set low_time is:%d\n", low_time);
			chip_config->low_time = low_time;
		} else if (!strncmp(buf, "cs_idletime=", 12)
			   && (1 == sscanf(buf + 12, "%d", &cs_idletime))) {
			OLED_PRINT("Set cs_idletime is:%d\n", cs_idletime);
			chip_config->cs_idletime = cs_idletime;
		} else if (!strncmp(buf, "ulthgh_thrsh=", 13)
			   && (1 == sscanf(buf + 13, "%d", &ulthgh_thrsh))) {
			OLED_PRINT("Set slwdown_thrsh is:%d\n", ulthgh_thrsh);
			chip_config->ulthgh_thrsh = ulthgh_thrsh;
		} else if (!strncmp(buf, "cpol=", 5) && (1 == sscanf(buf + 5, "%d", &cpol))) {
			OLED_PRINT("Set cpol is:%d\n", cpol);
			chip_config->cpol = cpol;
		} else if (!strncmp(buf, "cpha=", 5) && (1 == sscanf(buf + 5, "%d", &cpha))) {
			OLED_PRINT("Set cpha is:%d\n", cpha);
			chip_config->cpha = cpha;
		} else if (!strncmp(buf, "tx_mlsb=", 8) && (1 == sscanf(buf + 8, "%d", &tx_mlsb))) {
			OLED_PRINT("Set tx_mlsb is:%d\n", tx_mlsb);
			chip_config->tx_mlsb = tx_mlsb;
		} else if (!strncmp(buf, "rx_mlsb=", 8) && (1 == sscanf(buf + 8, "%d", &rx_mlsb))) {
			OLED_PRINT("Set rx_mlsb is:%d\n", rx_mlsb);
			chip_config->rx_mlsb = rx_mlsb;
		} else if (!strncmp(buf, "tx_endian=", 10)
			   && (1 == sscanf(buf + 10, "%d", &tx_endian))) {
			OLED_PRINT("Set tx_endian is:%d\n", tx_endian);
			chip_config->tx_endian = tx_endian;
		} else if (!strncmp(buf, "rx_endian=", 10)
			   && (1 == sscanf(buf + 10, "%d", &rx_endian))) {
			OLED_PRINT("Set rx_endian is:%d\n", rx_endian);
			chip_config->rx_endian = rx_endian;
		} else if (!strncmp(buf, "com_mod=", 8) && (1 == sscanf(buf + 8, "%d", &com_mod))) {
			chip_config->com_mod = com_mod;
			OLED_PRINT("Set com_mod is:%d\n", com_mod);
		} else if (!strncmp(buf, "pause=", 6) && (1 == sscanf(buf + 6, "%d", &pause))) {
			OLED_PRINT("Set pause is:%d\n", pause);
			chip_config->pause = pause;
		} else if (!strncmp(buf, "finish_intr=", 12)
			   && (1 == sscanf(buf + 12, "%d", &finish_intr))) {
			OLED_PRINT("Set finish_intr is:%d\n", finish_intr);
			chip_config->finish_intr = finish_intr;
		} else if (!strncmp(buf, "deassert=", 9) && (1 == sscanf(buf + 9, "%d", &deassert))) {
			OLED_PRINT("Set deassert is:%d\n", deassert);
			chip_config->deassert = deassert;
		} else if (!strncmp(buf, "ulthigh=", 8) && (1 == sscanf(buf + 8, "%d", &ulthigh))) {
			OLED_PRINT("Set ulthigh is:%d\n", ulthigh);
			chip_config->ulthigh = ulthigh;
		} else if (!strncmp(buf, "tckdly=", 7) && (1 == sscanf(buf + 7, "%d", &tckdly))) {
			OLED_PRINT("Set tckdly is:%d\n", tckdly);
			chip_config->tckdly = tckdly;
		} else if (!strncmp(buf, "sample_sel=", 11)
			   && (1 == sscanf(buf + 11, "%d", &sample_sel))) {
			OLED_PRINT("Set sample_sel is:%d\n", sample_sel);
			chip_config->sample_sel = sample_sel;
		} else if (!strncmp(buf, "cs_pol=", 7) && (1 == sscanf(buf + 7, "%d", &cs_pol))) {
			OLED_PRINT("Set cs_pol is:%d\n", cs_pol);
			chip_config->cs_pol = cs_pol;
		} else {
			OLED_LOG("Wrong parameters.\n");
			goto out;
		}
		spi->controller_data = chip_config;
		/* spi_setup(spi); */
	}
out:
	return count;
}

static int oled128x32_remove(struct spi_device *spi)
{
	oled128x32_delete_attr(&spi->dev);
	Oled_DeInit();
	#ifdef DIRECT_GPIO
	#else
		gpio_free(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX]);
		gpio_free(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX]);
	#endif
	
	return 0;
}

extern const struct file_operations oled128x32_fops;
static struct miscdevice oled128x32_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "oled128x32",
	.fops = &oled128x32_fops,
};

static int oled128x32_probe(struct spi_device *spi)
{
	struct mt_chip_conf *chip_config;

	int err;
	struct miscdevice *misc = &oled128x32_device;

	spi_oled128x32 = spi;

	#ifdef DIRECT_GPIO
		mt_set_gpio_mode(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], GPIO_LCD_RST_MODE);
		mt_set_gpio_dir(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], GPIO_DIR_OUT);
		mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], GPIO_OUT_ZERO);

		mt_set_gpio_mode(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], GPIO_LCD_DC_MODE);
		mt_set_gpio_dir(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], GPIO_DIR_OUT);
		mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], GPIO_OUT_ZERO);

	#else
		#if !defined(USE_LCM_RST)
			err = gpio_request(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], "GPIO_OLED_RST");
			if(err < 0) {
				OLED_PRINT( " GPIO_OLED_RST error = %d ", err);
				oled128x32_init_status = oled128x32_init_status | 0x02;
			} else {
				gpio_direction_output(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], 0);
			}
		#endif
		err = gpio_request(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], "GPIO_OLED_DC");
		if(err < 0) {
			OLED_PRINT( " GPIO_OLED_DC error = %d ", err);
			oled128x32_init_status = oled128x32_init_status | 0x04;
		} else {
			gpio_direction_output(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], 0);
		}
	#endif

	

	spi->mode = SPI_MODE_3;
	spi->bits_per_word = 8;

	chip_config = (struct mt_chip_conf *)spi->controller_data;

	if (!chip_config) {
		chip_config = kzalloc(sizeof(struct mt_chip_conf), GFP_KERNEL);
		if (!chip_config)
			return -ENOMEM;
	}
	chip_config->setuptime = 10000;
	chip_config->holdtime = 0;
	chip_config->high_time = 60; // 866 KHz
	chip_config->low_time = 60;
	chip_config->cs_idletime = 20;
	chip_config->ulthgh_thrsh = 0;
	chip_config->cpol = 1;
	chip_config->cpha = 1;
	chip_config->tx_mlsb = SPI_MSB;
	chip_config->rx_mlsb = SPI_MSB;
	chip_config->tx_endian = 0;
	chip_config->rx_endian = 0;
	chip_config->com_mod = FIFO_TRANSFER;
	chip_config->pause = 0;
	chip_config->finish_intr = 1;
	chip_config->deassert = 0;
	chip_config->ulthigh = 0;
	chip_config->tckdly = 0;
	chip_config->sample_sel = POSEDGE;
	chip_config->cs_pol = ACTIVE_LOW;

	Oled_Init();

	err = misc_register(misc);
	if (err) {
		OLED_LOG("register oled128x32\n");
		return err;
	}

	oled128x32_ptr->misc = misc;

	return oled128x32_create_attr(&spi->dev);
	return 0;
}

static struct spi_device_id oled128x32_id_table = { "spi-oled128x32", 0 };

static struct spi_driver oled128x32_spi_driver = {
	.driver = {
		   .name = "oled128x32_spi",
		   .bus = &spi_bus_type,
		   .owner = THIS_MODULE,
		   },
	.probe = oled128x32_probe,
	.remove = oled128x32_remove,
	.id_table = &oled128x32_id_table,
};

static struct spi_board_info oled128x32_spi_devs[] __initdata = {
	[0] = {
	       .modalias = "spi-oled128x32",
	       .bus_num = 0,
	       .chip_select = 1,
	       .mode = SPI_MODE_3,
	       },
};

static int __init oled128x32_init(void)
{
	#if (0)
		int status;
		status = register_chrdev(SPIDEV_MAJOR, "oled", &oled128x32_fops);
		if (status < 0)
			return status;
	#endif
	spi_register_board_info(oled128x32_spi_devs, ARRAY_SIZE(oled128x32_spi_devs));
	return spi_register_driver(&oled128x32_spi_driver);
}

static void __exit oled128x32_exit(void)
{
	spi_unregister_driver(&oled128x32_spi_driver);
	#if (0)
		unregister_chrdev(SPIDEV_MAJOR, oled128x32_spi_driver.driver.name);
	#endif
}

module_init(oled128x32_init);
module_exit(oled128x32_exit);

MODULE_AUTHOR("TCL XIAN ");
MODULE_DESCRIPTION("VG-2832TSWUG01 SPI Driver");
MODULE_LICENSE("GPL");
