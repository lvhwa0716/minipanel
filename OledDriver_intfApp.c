
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

#define DRIVER_HW_MODE	DRIVER_MODE_SPI // DRIVER_MODE_GPIO or DRIVER_MODE_SPI


#define OLED_DEVICE "/dev/oled128x32"

//#define OLED_USE_ATTR

#if (DRIVER_HW_MODE == DRIVER_MODE_GPIO)

	// "0" = sleep , "1" = wakeup
	#define OLED_POWER_NODE "/sys/class/misc/oled128x32/oled_power"
	// "XX" = bright , hex(upcase)
	#define OLED_BRIGHT_NODE "/sys/class/misc/oled128x32/oled_bright"

#elif (DRIVER_HW_MODE == DRIVER_MODE_SPI)
	// "0" = sleep , "1" = wakeup
	#define OLED_POWER_NODE "/sys/class/spi_master/spi0/spi0.1/oled_power"
	// "XX" = bright , hex(upcase)
	#define OLED_BRIGHT_NODE "/sys/class/spi_master/spi0/spi0.1/oled_bright"

	
#endif

#if defined(OLED_USE_ATTR)
	static int fd_OLED_POWER = -1;
	static int fd_OLED_BRIGHT = -1;
#endif
static int fd_OLED_DEVICE = -1;

void OledDriver_intfApp_Init(void)
{

	fd_OLED_DEVICE = open(OLED_DEVICE, O_RDWR);
	if(fd_OLED_DEVICE < 0){
		DBG_ERR("%s open failed \n" , OLED_DEVICE);
		return ;
	}
	#if defined(OLED_USE_ATTR)
		fd_OLED_POWER = open(OLED_POWER_NODE, O_RDWR);
		if(fd_OLED_POWER < 0){
			DBG_ERR( "%s open failed \n" , OLED_POWER);
			return ;
		}

		fd_OLED_BRIGHT = open(OLED_BRIGHT_NODE, O_RDWR);
		if(fd_OLED_BRIGHT < 0){
			DBG_ERR("%s open failed \n" , OLED_BRIGHT);
			return ;
		}
	#endif


}
void OledDriver_intfApp_DeInit(void)
{

	if(fd_OLED_DEVICE >=0 ) close(fd_OLED_DEVICE);
#if defined(OLED_USE_ATTR)
	if(fd_OLED_POWER >=0 ) close(fd_OLED_POWER);
	if(fd_OLED_BRIGHT >=0 ) close(fd_OLED_BRIGHT);
#endif
}



void OledDriver_intfApp_Sleep(void)
{
#if defined(OLED_USE_ATTR)
	static const char * CmdString = "0";
	if(fd_OLED_POWER < 0)
	{
		DBG_ERR("OLED_POWER not open");
		return ;
	}
	write(fd_OLED_POWER, CmdString, 2);
#else
	int off = 0;
	if(fd_OLED_DEVICE >= 0)
	{
		ioctl(fd_OLED_DEVICE, OLED_POWER, off);
	}
#endif
}
void OledDriver_intfApp_WakeUp(void)
{
#if defined(OLED_USE_ATTR)
	static const char * CmdString = "1";
	if(fd_OLED_POWER < 0)
	{
		DBG_ERR("OLED_POWER not open");
		return ;
	}
	write(fd_OLED_POWER, CmdString, 2);
#else
	int on = 1;
	if(fd_OLED_DEVICE >= 0)
	{
		ioctl(fd_OLED_DEVICE, OLED_POWER, on);
	}
#endif

}

void OledDriver_intfApp_Brightness(int b)
{
#if defined(OLED_USE_ATTR)
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
#else
	if(fd_OLED_DEVICE >= 0)
	{
		ioctl(fd_OLED_DEVICE, OLED_BRIGHTNESS, b);
	}
#endif

}

static unsigned char DRAM_V_FB[128 * 32 / 8];
#if (MICROPANEL_BPP == 1)
	static const unsigned char src_mask[8] = {
		0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
#endif
static const unsigned char dst_mask[8] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

static unsigned char * __Oled_Convert(unsigned char *pBuf) // HORIZONTAL => VERTICAL
{
	int i, j;

	memset(DRAM_V_FB, 0, sizeof(DRAM_V_FB));
	for( i = 0 ; i < 4; i++)
	{
		for( j = 0 ; j < 128; j++)
		{
			unsigned char dst_data = 0;
			int raw_idx = 0;
			for(raw_idx = 0; raw_idx < 8; raw_idx++)
			{
				#if (MICROPANEL_BPP == 1)
					if(*(pBuf + i * 8 * 32 + raw_idx * 32 + (j >> 3)) & src_mask[j & 0x7])
					{
						dst_data = dst_data | dst_mask[j & 0x7];
					}
				#elif (MICROPANEL_BPP == 8)
					if(*(pBuf + i * 8 * 32 * MICROPANEL_BPP + raw_idx * 32 * MICROPANEL_BPP + j) == 0)
					{
						dst_data = dst_data | dst_mask[j & 0x7];
					}
				#endif
			}
			DRAM_V_FB[ i * 128 + j] = dst_data;
		}
	}
	return DRAM_V_FB;
}

void OledDriver_intfApp_Update(unsigned char *pBuf, int x, int y, int w, int h)
{
	pBuf = __Oled_Convert(pBuf);
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



