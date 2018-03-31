#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <generated/autoconf.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/atomic.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#include "mt-plat/mtgpio.h"
#include <linux/types.h>
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


static void drv_udelay(int usec)
{
	udelay(usec);
}


void Reset_Command(void)
{
	OledDriver_SetPin(GPIO_LCD_RST_IDX , LOW_LEVEL); //RES=0;
	mdelay(300);
	OledDriver_SetPin(GPIO_LCD_RST_IDX , HIGH_LEVEL);//RES=1;
	mdelay(300);
}
void Write_Command(unsigned char Data)
{
	unsigned char i;

	OledDriver_SetPin(GPIO_LCD_CS_IDX , LOW_LEVEL); // CS=0;
	OledDriver_SetPin(GPIO_LCD_DC_IDX , LOW_LEVEL); //DC=0;
	for (i=0; i<8; i++)
	{
		OledDriver_SetPin(GPIO_LCD_SCLK_IDX , LOW_LEVEL);//SCLK=0;
		OledDriver_SetPin(GPIO_LCD_SDIN_IDX , (Data&0x80)>>7);//SDIN=(Data&0x80)>>7;
		Data = Data << 1;
		drv_udelay(1);
		OledDriver_SetPin(GPIO_LCD_SCLK_IDX , HIGH_LEVEL);//SCLK=1;
		drv_udelay(1);
	}
	//	SCLK=0;
	OledDriver_SetPin(GPIO_LCD_DC_IDX , HIGH_LEVEL); //DC=1;
	OledDriver_SetPin(GPIO_LCD_CS_IDX , HIGH_LEVEL); //CS=1;
}
void Write_Data(unsigned char Data)
{
	unsigned char i;

	OledDriver_SetPin(GPIO_LCD_CS_IDX , LOW_LEVEL); //CS=0;
	OledDriver_SetPin(GPIO_LCD_DC_IDX , HIGH_LEVEL); //DC=1;
	for (i=0; i<8; i++)
	{
		OledDriver_SetPin(GPIO_LCD_SCLK_IDX , LOW_LEVEL);//SCLK=0;
		OledDriver_SetPin(GPIO_LCD_SDIN_IDX , (Data&0x80)>>7);//SDIN=(Data&0x80)>>7;
		Data = Data << 1;
		drv_udelay(1);
		OledDriver_SetPin(GPIO_LCD_SCLK_IDX , HIGH_LEVEL);//SCLK=1;
		drv_udelay(1);
	}
	//	SCLK=0;
	OledDriver_SetPin(GPIO_LCD_DC_IDX , HIGH_LEVEL); //DC=1;
	OledDriver_SetPin(GPIO_LCD_CS_IDX , HIGH_LEVEL); //CS=1;
}

void WriteMultiData(unsigned char *pBuf, int len)
{
	int j ;
	for(j=0;j<len;j++)
	{
		Write_Data(*pBuf);
		pBuf++;
	}
}

/*----------------------------------------------------------------------------*/
static struct miscdevice oled128x32_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "oled128x32",
	.fops = &oled128x32_fops,
};

/*---------------------------------------------------------------------------*/
static int oled128x32_probe(struct platform_device *dev)
{
	int err = 0;
	struct miscdevice *misc = &oled128x32_device;
	memset(oled128x32_ptr->frame_buffer, 0 , OLED_FRAMEBUFFER_LENGTH);

	#ifdef DIRECT_GPIO
		mt_set_gpio_mode(OLEDDRIVER_PIN_INDEX[GPIO_LCD_CS_IDX], GPIO_LCD_CS_MODE);
		mt_set_gpio_dir(OLEDDRIVER_PIN_INDEX[GPIO_LCD_CS_IDX], GPIO_DIR_OUT);
		mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_CS_IDX], GPIO_OUT_ONE);

		mt_set_gpio_mode(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], GPIO_LCD_RST_MODE);
		mt_set_gpio_dir(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], GPIO_DIR_OUT);
		mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], GPIO_OUT_ZERO);

		mt_set_gpio_mode(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], GPIO_LCD_DC_MODE);
		mt_set_gpio_dir(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], GPIO_DIR_OUT);
		mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX], GPIO_OUT_ZERO);

		mt_set_gpio_mode(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SCLK_IDX], GPIO_LCD_SCLK_MODE);
		mt_set_gpio_dir(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SCLK_IDX], GPIO_DIR_OUT);
		mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SCLK_IDX], GPIO_OUT_ZERO);

		mt_set_gpio_mode(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SDIN_IDX], GPIO_LCD_SDIN_MODE);
		mt_set_gpio_dir(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SDIN_IDX], GPIO_DIR_OUT);
		mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SDIN_IDX], GPIO_OUT_ZERO);
	#else
		err = gpio_request(OLEDDRIVER_PIN_INDEX[GPIO_LCD_CS_IDX], "GPIO_OLED_CS");
		if(err < 0) {
			OLED_PRINT( " GPIO_OLED_CS error = %d ", err);
			oled128x32_init_status = oled128x32_init_status | 0x01;
		} else {
			gpio_direction_output(OLEDDRIVER_PIN_INDEX[GPIO_LCD_CS_IDX], 1);
		}
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

		err = gpio_request(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SCLK_IDX], "GPIO_OLED_SCLK");
		if(err < 0) {
			OLED_PRINT( " GPIO_OLED_SCLK error = %d ", err);
			oled128x32_init_status = oled128x32_init_status | 0x08;
		} else {
			gpio_direction_output(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SCLK_IDX], 0);
		}

		err = gpio_request(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SDIN_IDX], "GPIO_OLED_SDIN");
		if(err < 0) {
			OLED_PRINT( " GPIO_OLED_SDIN error = %d ", err);
			oled128x32_init_status = oled128x32_init_status | 0x10;
		} else {
			gpio_direction_output(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SDIN_IDX], 0);
		}
	#endif
	Oled_Init();
	OLED_LOG("Registering oled128x32 device\n");

	err = misc_register(misc);
	if (err) {
		OLED_LOG("register oled128x32\n");
		return err;
	}

	oled128x32_ptr->misc = misc;

	platform_set_drvdata(dev, oled128x32_ptr);

	err = oled128x32_create_attr(misc->this_device);
	if (err)
		OLED_LOG("create attribute\n");

	

	return err;
}

/*---------------------------------------------------------------------------*/
static int oled128x32_remove(struct platform_device *dev)
{

	int err;
	struct oled128x32_obj_t *obj = platform_get_drvdata(dev);
	if (obj == NULL) {
		OLED_LOG("oled128x32_obj_t is NULL\n");
		return 0;
	}
	err = oled128x32_delete_attr(obj->misc->this_device);
	if (err)
		OLED_LOG("delete attr\n");

	err = misc_deregister(obj->misc);
	if (err)
		OLED_LOG("deregister oled128x32\n");

	Oled_DeInit();

	#ifdef DIRECT_GPIO
	#else
		gpio_free(OLEDDRIVER_PIN_INDEX[GPIO_LCD_CS_IDX]);
		gpio_free(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX]);
		gpio_free(OLEDDRIVER_PIN_INDEX[GPIO_LCD_DC_IDX]);
		gpio_free(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SCLK_IDX]);
		gpio_free(OLEDDRIVER_PIN_INDEX[GPIO_LCD_SDIN_IDX]);
	#endif
	
	return err;
}


/*---------------------------------------------------------------------------*/


static struct platform_driver oled128x32_driver = {
	.probe = oled128x32_probe,
	.remove = oled128x32_remove,
	.driver = {
		   .name = "oled128x32",
		   .owner = THIS_MODULE,
		   },
};


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#ifdef CONFIG_OF
static struct platform_device oled128x32_platform_device = {
	.name = "oled128x32",
	.id = -1
};

#endif
static int __init oled128x32_init(void)
{
	int ret = 0;

#ifdef CONFIG_OF
	ret = platform_device_register(&oled128x32_platform_device);
	if (ret)
		OLED_LOG("oled128x32_init:dev:E%d\n", ret);
#endif

	ret = platform_driver_register(&oled128x32_driver);
	return ret;
}

/*---------------------------------------------------------------------------*/
static void __exit oled128x32_exit(void)
{
	platform_driver_unregister(&oled128x32_driver);
}


/*---------------------------------------------------------------------------*/
module_init(oled128x32_init);
module_exit(oled128x32_exit);
MODULE_AUTHOR("TCL XIAN ");
MODULE_DESCRIPTION("VG-2832TSWUG01 GPIO Driver");
MODULE_LICENSE("GPL");
/*---------------------------------------------------------------------------*/
