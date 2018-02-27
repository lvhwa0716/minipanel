#ifndef __OLEDMONOIOCTL_H__
#define __OLEDMONOIOCTL_H__

#include <asm/ioctl.h>

struct oled_rect {
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
};

#define OLED_SLEEP		_IO('O', 0x01)
#define OLED_WAKEUP 	_IO('O', 0x02)
#define OLED_BRIGHTNESS	_IOW('O', 0x10, int)
#define OLED_UPDATERECT	_IOW('O', 0x10, struct oled_rect)
#define OLED_FILLFB		_IOW('O', 0x10, unsigned char[128 * 32 / 8])

#endif // __OLEDMONOIOCTL_H__

