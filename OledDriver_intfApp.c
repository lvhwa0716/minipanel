
#define LOG_TAG "MicroPanelService_intf"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "MicroPanelGui.h"
#include "OledMonoIoctl.h"




//#define OLED_USE_ATTR


#define OLED_DEVICE "/dev/oled128x32"

// "0" = sleep , "1" = wakeup
#define OLED_POWER_NODE "sys/class/ssd1316/oled128x32/oled_power"
// "XX" = bright , hex(upcase)
#define OLED_BRIGHT_NODE "/sys/class/ssd1316/oled128x322/oled_bright"



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
	if(fd_OLED_DEVICE >= 0)
	{
		ioctl(fd_OLED_DEVICE, OLED_POWER, OLED_POWER_OFF);
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
	if(fd_OLED_DEVICE >= 0)
	{
		ioctl(fd_OLED_DEVICE, OLED_POWER, OLED_POWER_ON);
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

static const unsigned char dst_mask_[8] = {0x01, 0x02 , 0x04 ,0x08,0x10 ,0x20,0x40,0x80};
static const unsigned char src_mask_[8] = { 0x80,0x40, 0x20 ,0x10 ,0x08,0x04,0x02,0x01 };

#define getpixel(buf, x, y) (buf[((y) * 16) + ((x) >> 3)] & src_mask_[(x) & 0x7])
#define setpixel(buf, x, y) (buf[(x) + ((y) >> 3 ) * 128] |= dst_mask_[(y) & 0x7])
static unsigned char * __Oled_Convert(unsigned char *pBuf)
{
	int y, x;
	
	static unsigned char DRAM_V_FB[128 * 32 / 8];
	memset(DRAM_V_FB, 0, sizeof(DRAM_V_FB));
	for( y = 0; y < 32 ; y++ ) {
        for( x = 0; x < 128  ; x++ ) {
			#if 0 
				// 1bpp
		        if( 0 != getpixel(pBuf, x , y) )
		        	setpixel(DRAM_V_FB, x , y);
			#else
				// 8bpp
				if( 0 != (pBuf[x , y * 128] & 0x80) )
		        	setpixel(DRAM_V_FB, x , y);
			#endif
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
		struct oled_framebuffer fb;
		int ret = 0;
		fb.size = OLED_FRAMEBUFFER_SIZE;
		fb.pbuf = pBuf;
		ret = ioctl(fd_OLED_DEVICE, OLED_FILLFB, &fb);
		DBG_LOG("OLED_DEVICE fill framebuffer: %d" , ret);
		_rect.x = (unsigned int)x;
		_rect.y = (unsigned int)y;
		_rect.w = (unsigned int)w;
		_rect.h = (unsigned int)h;
		ret = ioctl(fd_OLED_DEVICE, OLED_UPDATERECT, &_rect);
		DBG_LOG("OLED_DEVICE update rect: %d" , ret);
	} else {
		DBG_LOG("fd_OLED_DEVICE error: %d" , fd_OLED_DEVICE);
	}
}



