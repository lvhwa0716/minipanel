#ifndef __OLEDMONO_H__
#define __OLEDMONO_H__

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

#include <linux/types.h>

#define OLED_FRAMEBUFFER_LENGTH (MICROPANEL_BPP * (128 * 32 / 8))	// 128 x 32 , white/black

struct oled128x32_obj_t {
	atomic_t ref;
	/*
	
	dev_t devno;
	struct class *cls;
	struct device *dev;
	struct cdev chrdev;
	*/
	struct miscdevice *misc;
	
	int power_state;
	int brightness;
	int need_update;
	char frame_buffer[OLED_FRAMEBUFFER_LENGTH];
};

#define oled128x32_rectUpdate_headSize	(sizeof(unsigned short) * 6)
struct oled128x32_rectUpdate {
	short x;
	short y;
	unsigned short w;
	unsigned short h;
	unsigned short bpl;
	unsigned short size; // frame_buffer length
	char frame_buffer[];
}__attribute__((packed));

#define OLED_PRINT(fmt, args...) pr_err("[oled128x32]: [%s]:[%d]" fmt, __func__, __LINE__, ##args)

//#define DIRECT_GPIO // GPIO19 HARDCODE warning!!! mt_gpio_affix.c

#if (1)
	#define OLED_LOG(fmt, args...) pr_err("[oled128x32]: [%s]:[%d]" fmt, __func__, __LINE__, ##args)
#else
	#define OLED_LOG(fmt, args...) while(0)
#endif



#endif // #ifndef __OLEDMONO_H__
