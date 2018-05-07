#ifndef __OLEDMONOIOCTL_H__
#define __OLEDMONOIOCTL_H__

#include <asm/ioctl.h>

#define OLED_POWER_ON	1
#define OLED_POWER_OFF	0
#define OLED_FRAMEBUFFER_SIZE (128 * 32 / 8)

struct oled_rect {
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
};

struct oled_framebuffer {
	int size;
	unsigned char * pbuf;
};

#define dup_SSD1316_IOC_MAGIC	'g'

#define OLED_RESET		_IOW(dup_SSD1316_IOC_MAGIC, 0x10, int)
#define OLED_POWER		_IOW(dup_SSD1316_IOC_MAGIC, 0x11, int)
#define OLED_BRIGHTNESS	_IOW(dup_SSD1316_IOC_MAGIC, 0x12, int)
#define OLED_UPDATERECT	_IOW(dup_SSD1316_IOC_MAGIC, 0x13, struct oled_rect)
#define OLED_FILLFB		_IOW(dup_SSD1316_IOC_MAGIC, 0x14, struct oled_framebuffer)

#endif // __OLEDMONOIOCTL_H__

