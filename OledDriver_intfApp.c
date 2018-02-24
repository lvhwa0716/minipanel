

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

extern const int GPIO_LCD_CS_IDX;
extern const int GPIO_LCD_RST_IDX;
extern const int GPIO_LCD_DC_IDX;
extern const int GPIO_LCD_SCLK_IDX;
extern const int GPIO_LCD_SDIN_IDX;
extern const unsigned int OLEDDRIVER_PIN_INDEX[];
extern const int LOW_LEVEL;
extern const int HIGH_LEVEL;

extern void Oled_Init(void);
extern void Oled_DeInit(void);
extern void Oled_Sleep(void);
extern void Oled_WakeUp(void);
extern void Oled_Brightness(int b);
extern void Oled_UpdateAll(unsigned char *pBuf);

#define DRIVER_MODE_APP	0
#define DRIVER_MODE_GPIO	1
#define DRIVER_MODE_SPI	2

#define DRIVER_HW_MODE	DRIVER_MODE_GPIO // DRIVER_MODE_GPIO or DRIVER_MODE_SPI

// DRIVER_SW_MODE must set DRIVER_MODE_APP when release , when not DRIVER_MODE_APP , OLED_BUFFER is invalid 
#define DRIVER_SW_MODE	DRIVER_MODE_APP//DRIVER_MODE_GPIO // DRIVER_MODE_APP or same as DRIVER_HW_MODE


// Check define
#if (DRIVER_HW_MODE == 0)
	#error "DRIVER_HW_MODE must DRIVER_MODE_GPIO or SPI"
#endif

#if (DRIVER_SW_MODE != 0)
	#if (DRIVER_SW_MODE != DRIVER_HW_MODE)
		#error " When Test Driver , DRIVER_SW_MODE must eq DRIVER_HW_MODE"
	#endif
#endif


#if (DRIVER_HW_MODE == DRIVER_MODE_GPIO)
	//"-px y" => x: pin number , y = level
	//-px 0|1   , x=0 RST ; 1 DC ; 2 CS; 3 SCLK; 4 SDIN
	#define PIN_CTRL "/sys/class/misc/oled128x32/pin"

	// RAW DATA , RECT
	#define OLED_BUFFER_RECT "/sys/class/misc/oled128x32/oled_rawrect"

	// RAW DATA , must full screen
	#define OLED_BUFFER "/sys/class/misc/oled128x32/oled_raw"

	// "0" = sleep , "1" = wakeup
	#define OLED_POWER "/sys/class/misc/oled128x32/oled_power"
	// "XX" = bright , hex(upcase)
	#define OLED_BRIGHT "/sys/class/misc/oled128x32/oled_bright"

#elif (DRIVER_HW_MODE == DRIVER_MODE_SPI)
	//"-px y" => x: pin number , y = level
	//-px 0|1   , x=0 RST ; 1 DC ; 2 CS; 3 SCLK; 4 SDIN
	#define PIN_CTRL "/sys/class/spi_master/spi0/spi0.1/pin"

	#define SPI_RAW "/sys/class/spi_master/spi0/spi0.1/spi_raw"
	#define SPI_CFG "/sys/class/spi_master/spi0/spi0.1/spi_cfg"

	// RAW DATA , RECT
	#define OLED_BUFFER_RECT "/sys/class/spi_master/spi0/spi0.1/oled_rawrect"

	// RAW DATA , must full screen
	#define OLED_BUFFER "/sys/class/spi_master/spi0/spi0.1/oled_raw"

	// "0" = sleep , "1" = wakeup
	#define OLED_POWER "/sys/class/spi_master/spi0/spi0.1/oled_power"
	// "XX" = bright , hex(upcase)
	#define OLED_BRIGHT "/sys/class/spi_master/spi0/spi0.1/oled_bright"
#endif


static void drv_udelay(int usec)
{
	usleep(usec);
}

#if (DRIVER_SW_MODE == DRIVER_MODE_APP)
	static int fd_OLED_BUFFER = -1;
	static int fd_OLED_POWER = -1;
	static int fd_OLED_BRIGHT = -1;
	// following not use
	void Reset_Command(void)
	{
	}
	void Write_Command(unsigned char Data)
	{
		Data = Data;
	}
	void Write_Data(unsigned char Data)
	{
		Data = Data;
	}
	void WriteMultiData(unsigned char *pBuf, int len)
	{
		pBuf = pBuf;
		len = len;
	}
#elif (DRIVER_SW_MODE == DRIVER_MODE_GPIO)
	static int fdPinCtrl = -1;
	void OledDriver_SetPin(int pin_idx, int level)
	{
		char buf[16];
		int size = snprintf(buf,15,"-p%d %d",pin_idx,level);
		write(fdPinCtrl, buf, size + 1);
	}
	void Reset_Command(void)
	{
		OledDriver_SetPin(GPIO_LCD_RST_IDX , LOW_LEVEL); //RES=0;
		sleep(1);
		OledDriver_SetPin(GPIO_LCD_RST_IDX , HIGH_LEVEL);//RES=1;
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
#elif (DRIVER_SW_MODE == DRIVER_MODE_SPI)
	static int fdSpiCtrl = -1;
	static int fdSpiCfg = -1;
	static int fdPinCtrl = -1;

	void OledDriver_SetPin(int pin_idx, int level)
	{
		char buf[16];
		int size = snprintf(buf,15,"-p%d %d",pin_idx,level);
		write(fdPinCtrl, buf, size + 1);
	}
	void Reset_Command(void)
	{
		OledDriver_SetPin(GPIO_LCD_RST_IDX , LOW_LEVEL); //RES=0;
		sleep(1);
		OledDriver_SetPin(GPIO_LCD_RST_IDX , HIGH_LEVEL);//RES=1;
	}
	void Write_Command(unsigned char Data)
	{
		unsigned char pBuf[2];
		pBuf[0] = Data;
		OledDriver_SetPin(GPIO_LCD_DC_IDX , LOW_LEVEL); //RES=0;
		drv_udelay(1);
		write(fdSpiCtrl, pBuf, 1);
	}
	void Write_Data(unsigned char Data)
	{
		unsigned char pBuf[2];
		pBuf[0] = Data;
		OledDriver_SetPin(GPIO_LCD_DC_IDX , HIGH_LEVEL); //RES=0;
		drv_udelay(1);
		write(fdSpiCtrl, pBuf, 1);
	}
	#define OLED_BURST	32
	void WriteMultiData(unsigned char *pBuf, int len)
	{
		int j ;
		int sent_size = 0;
		OledDriver_SetPin(GPIO_LCD_DC_IDX , HIGH_LEVEL); //RES=0;
		drv_udelay(1);
		for(j=0;j< (len / OLED_BURST) ;j++)
		{
			write(fdSpiCtrl, pBuf, OLED_BURST);
			pBuf = pBuf + OLED_BURST;
			sent_size =  sent_size + OLED_BURST;
		}
		if( len > sent_size)
		{
			write(fdSpiCtrl, pBuf, len - sent_size);
		}
	}
#endif


void OledDriver_intfApp_Init(void)
{
	#if (DRIVER_SW_MODE == DRIVER_MODE_APP)
		fd_OLED_BUFFER = open(OLED_BUFFER, O_RDWR);
		if(fd_OLED_BUFFER < 0){
			#ifdef DEBUG_LOG
				fprintf ( stderr, "%s open failed \n" , OLED_BUFFER);
			#endif
			return ;
		}

		fd_OLED_POWER = open(OLED_POWER, O_RDWR);
		if(fd_OLED_POWER < 0){
			#ifdef DEBUG_LOG
				fprintf ( stderr, "%s open failed \n" , OLED_POWER);
			#endif
			return ;
		}

		fd_OLED_BRIGHT = open(OLED_BRIGHT, O_RDWR);
		if(fd_OLED_BRIGHT < 0){
			#ifdef DEBUG_LOG
				fprintf ( stderr, "%s open failed \n" , OLED_BRIGHT);
			#endif
			return ;
		}
	#elif (DRIVER_SW_MODE == DRIVER_MODE_GPIO)
		fdPinCtrl = open(PIN_CTRL, O_WRONLY);
		if(fdPinCtrl < 0){
			#ifdef DEBUG_LOG
				fprintf ( stderr, "%s open failed \n" , PIN_CTRL);
			#endif
			return ;
		}
		Oled_Init();
	#elif (DRIVER_SW_MODE == DRIVER_MODE_SPI)
		fdPinCtrl = open(PIN_CTRL, O_WRONLY);
		if(fdPinCtrl < 0){
			#ifdef DEBUG_LOG
				fprintf ( stderr, "%s open failed \n" , PIN_CTRL);
			#endif
			return ;
		}
		fdSpiCtrl = open(SPI_RAW, O_RDWR);
		if(fdSpiCtrl < 0){
			#ifdef DEBUG_LOG
				fprintf ( stderr, "%s open failed \n" , SPI_RAW);
			#endif
			return ;
		}
		fdSpiCfg = open(SPI_CFG, O_WRONLY);
		if(fdSpiCfg < 0){
			#ifdef DEBUG_LOG
				fprintf ( stderr, "%s open failed \n" , SPI_CFG);
			#endif
			return ;
		}
		Oled_Init();
	#endif

}
void OledDriver_intfApp_DeInit(void)
{
	#if (DRIVER_SW_MODE == DRIVER_SW_MODE_APP)
		if(fd_OLED_BUFFER >=0 ) close(fd_OLED_BUFFER);
		if(fd_OLED_POWER >=0 ) close(fd_OLED_POWER);
		if(fd_OLED_BRIGHT >=0 ) close(fd_OLED_BRIGHT);
	#elif (DRIVER_SW_MODE == DRIVER_MODE_GPIO)
		Oled_DeInit();
		if(fdPinCtrl >=0 ) close(fdPinCtrl);
		
	#elif (DRIVER_SW_MODE == DRIVER_MODE_SPI)
		Oled_DeInit();
		if(fdPinCtrl >=0 ) close(fdPinCtrl);
		if(fdSpiCtrl >=0 ) close(fdSpiCtrl);
		if(fdSpiCfg >=0 ) close(fdSpiCfg);
	#endif
}



void OledDriver_intfApp_Sleep(void)
{
	#if (DRIVER_SW_MODE == DRIVER_SW_MODE_APP)
		static const char * CmdString = "0";
		write(fd_OLED_POWER, CmdString, 2);
	#else
		Oled_Sleep();
	#endif
}
void OledDriver_intfApp_WakeUp(void)
{
	#if (DRIVER_SW_MODE == DRIVER_SW_MODE_APP)
		static const char * CmdString = "1";
		write(fd_OLED_POWER, CmdString, 2);
	#else
		Oled_WakeUp();
	#endif
}

void OledDriver_intfApp_Brightness(int b)
{
	#if (DRIVER_SW_MODE == DRIVER_SW_MODE_APP)
		char CmdString[16];
		int size;
		b = b & 0xFF; // only 255
		size = snprintf(CmdString, 15, "%02X", b);
		write(fd_OLED_POWER, CmdString, size + 1);
	#else
		Oled_Brightness(b);
	#endif
}
void OledDriver_intfApp_UpdateAll(unsigned char *pBuf)
{
	#if (DRIVER_SW_MODE == DRIVER_SW_MODE_APP)
		write(fd_OLED_BUFFER, pBuf, 128 * 32 / 8);
	#else
		Oled_UpdateAll(pBuf);
	#endif
}



