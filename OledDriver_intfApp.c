
#define LOG_TAG "MicroPanelService_intf"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "MicroPanelGui.h"
#include "OledMonoIoctl.h"

#define DRIVER_MODE_GPIO	1
#define DRIVER_MODE_SPI	2

#define DRIVER_HW_MODE	DRIVER_MODE_GPIO // DRIVER_MODE_GPIO or DRIVER_MODE_SPI


#define OLED_DEVICE "/dev/oled128x32"

#if (DRIVER_HW_MODE == DRIVER_MODE_GPIO)

	// "0" = sleep , "1" = wakeup
	#define OLED_POWER "/sys/class/misc/oled128x32/oled_power"
	// "XX" = bright , hex(upcase)
	#define OLED_BRIGHT "/sys/class/misc/oled128x32/oled_bright"

#elif (DRIVER_HW_MODE == DRIVER_MODE_SPI)
	// "0" = sleep , "1" = wakeup
	#define OLED_POWER "/sys/class/spi_master/spi0/spi0.1/oled_power"
	// "XX" = bright , hex(upcase)
	#define OLED_BRIGHT "/sys/class/spi_master/spi0/spi0.1/oled_bright"

	
#endif

static int fd_OLED_POWER = -1;
static int fd_OLED_BRIGHT = -1;
static int fd_OLED_DEVICE = -1;

void OledDriver_intfApp_Init(void)
{

	fd_OLED_DEVICE = open(OLED_DEVICE, O_RDWR);
	if(fd_OLED_DEVICE < 0){
		DBG_ERR("%s open failed \n" , OLED_DEVICE);
		return ;
	}

	fd_OLED_POWER = open(OLED_POWER, O_RDWR);
	if(fd_OLED_POWER < 0){
		DBG_ERR( "%s open failed \n" , OLED_POWER);
		return ;
	}

	fd_OLED_BRIGHT = open(OLED_BRIGHT, O_RDWR);
	if(fd_OLED_BRIGHT < 0){
		DBG_ERR("%s open failed \n" , OLED_BRIGHT);
		return ;
	}


}
void OledDriver_intfApp_DeInit(void)
{

	if(fd_OLED_DEVICE >=0 ) close(fd_OLED_DEVICE);
	if(fd_OLED_POWER >=0 ) close(fd_OLED_POWER);
	if(fd_OLED_BRIGHT >=0 ) close(fd_OLED_BRIGHT);
}



void OledDriver_intfApp_Sleep(void)
{
	static const char * CmdString = "0";
	if(fd_OLED_POWER < 0)
	{
		DBG_ERR("OLED_POWER not open");
		return ;
	}
	write(fd_OLED_POWER, CmdString, 2);

}
void OledDriver_intfApp_WakeUp(void)
{

	static const char * CmdString = "1";
	if(fd_OLED_POWER < 0)
	{
		DBG_ERR("OLED_POWER not open");
		return ;
	}
	write(fd_OLED_POWER, CmdString, 2);

}

void OledDriver_intfApp_Brightness(int b)
{

	char CmdString[16];
	int size;
	b = b & 0xFF; // only 255
	if(fd_OLED_BRIGHT < 0)
	{
		DBG_ERR("OLED_BRIGHT not open");
		return ;
	}
	size = snprintf(CmdString, 15, "%02X", b);
	write(fd_OLED_BRIGHT, CmdString, size + 1);

}
void OledDriver_intfApp_Update(unsigned char *pBuf, int x, int y, int w, int h)
{

	if(fd_OLED_DEVICE >= 0)
	{
		struct oled_rect _rect;
		int ret = 0;
		_rect.x = (unsigned int)x;
		_rect.y = (unsigned int)y;
		_rect.w = (unsigned int)w;
		_rect.h = (unsigned int)h;
		ret = ioctl(fd_OLED_DEVICE, OLED_FILLFB, pBuf);
		DBG_LOG("OLED_DEVICE fill framebuffer: %d" , ret);
		ret = ioctl(fd_OLED_DEVICE, OLED_UPDATERECT, &_rect);
		DBG_LOG("OLED_DEVICE update rect: %d" , ret);
	} else {
		DBG_LOG("fd_OLED_DEVICE error: %d" , fd_OLED_DEVICE);
	}
}



