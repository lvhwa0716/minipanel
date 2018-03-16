

#include "OledMono.h"
#include "OledMonoIoctl.h"

// mdelay udelay ndelay

extern const int Brightness;
extern void Oled_Sleep(void);
extern void Oled_WakeUp(void);
extern void Oled_Brightness(int b);
extern void Oled_UpdateAll(unsigned char *pBuf);
extern void Oled_UpdateRect(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned char *pBuf);




static struct oled128x32_obj_t oled128x32_private = {
	.power_state = 1,
	.brightness = -1,
	.need_update = 0,
	/* .lock = __SPIN_LOCK_UNLOCKED(die.lock), */
};


struct oled128x32_obj_t *oled128x32_ptr = &oled128x32_private;

static DEFINE_MUTEX(oled_mutex);

int oled128x32_init_status = 0;

extern void OledDriver_SetPin(int pin, int level);


/*****************************************************************************/
/* File operation                                                            */
/*****************************************************************************/
static int oled128x32_open(struct inode *inode, struct file *file)
{
	struct oled128x32_obj_t *obj = oled128x32_ptr;


	if (obj == NULL) {
		OLED_LOG("NULL pointer");
		return -EFAULT;
	}

	atomic_inc(&obj->ref);
	file->private_data = obj;
	return nonseekable_open(inode, file);
}

/*---------------------------------------------------------------------------*/
static int oled128x32_release(struct inode *inode, struct file *file)
{
	struct oled128x32_obj_t *obj = file->private_data;

	if (obj == NULL) {
		OLED_LOG("NULL pointer");
		return -EFAULT;
	}

	atomic_dec(&obj->ref);
	return 0;
}

/*---------------------------------------------------------------------------*/
static long oled128x32_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct oled128x32_obj_t *obj = file->private_data;

	OLED_LOG("cmd = %08X" , cmd);
	
	switch (cmd) {
		case OLED_SLEEP:
		case OLED_WAKEUP: {
			unsigned char state;
			mutex_lock(&oled_mutex);
			// do any op here
			state = (cmd == OLED_SLEEP ? 0 : 1);

			if(state == 0)
			{
				Oled_Sleep();
			}
			else
			{
				Oled_WakeUp();
				if(oled128x32_ptr->need_update == 1)
				{
					Oled_UpdateAll((unsigned char *)obj->frame_buffer);
					obj->need_update = 0;
				}
			}
			obj->power_state = state;
			mutex_unlock(&oled_mutex);
			break;
		}
		case OLED_BRIGHTNESS: {
			int brightness = 0;
			if(copy_from_user(&brightness, (int*)arg, sizeof(int)))
				return -EFAULT;
			mutex_lock(&oled_mutex);
			// do any op here
			Oled_Brightness((unsigned char)(brightness & 0xFF));
			obj->brightness = brightness;

			mutex_unlock(&oled_mutex);
			break;
		}
		case OLED_UPDATERECT: {
			struct oled_rect _oled_rect;
			unsigned short x1,y1,x2,y2;
			if (copy_from_user(&_oled_rect, (struct oled_rect*)arg, sizeof(struct oled_rect))) {
				OLED_LOG(" copy framebuff error\n");
				return -EFAULT;
			}
			x1 = _oled_rect.x;
			y1 = _oled_rect.y;
			x2 = _oled_rect.x + _oled_rect.w;
			y2 = _oled_rect.y + _oled_rect.h;
			if( (x2 > 127) || (y2 > 31))
				return -EINVAL;
			mutex_lock(&oled_mutex);
			if(obj->power_state == 1) {
				Oled_UpdateRect(x1, y1, x2, y2, obj->frame_buffer);
				obj->need_update = 0;
			}
			mutex_unlock(&oled_mutex);
			break;
		}
		case OLED_FILLFB: {
			if (copy_from_user(obj->frame_buffer, (unsigned char*)arg, sizeof(unsigned char) * OLED_FRAMEBUFFER_LENGTH))
				return -EFAULT;
			obj->need_update = 1;
			break;
		}
		
		default:{
			OLED_LOG(" cmd = 0x%8X, not define\n" , cmd);
			return -EINVAL;
			break;
		}
	}

	return 0;
}
static ssize_t oled128x32_read (struct file * file, char __user * buf, size_t count,
			loff_t * ppos)
{
	struct oled128x32_obj_t *obj = file->private_data;

	if (count == 0)
		return 0;
	if(count > sizeof(unsigned char) * OLED_FRAMEBUFFER_LENGTH)
		count = sizeof(unsigned char) * OLED_FRAMEBUFFER_LENGTH;

	if (copy_to_user (buf, obj->frame_buffer, count))
	{
		return -EFAULT;
	}
	return count;
}

static ssize_t oled128x32_write (struct file * file, const char __user * buf,
			 size_t count, loff_t * ppos)
{
	struct oled128x32_obj_t *obj = file->private_data;

	if (count == 0)
		return 0;

	if(count > sizeof(unsigned char) * OLED_FRAMEBUFFER_LENGTH)
		count = sizeof(unsigned char) * OLED_FRAMEBUFFER_LENGTH;

	if (copy_from_user (obj->frame_buffer, buf, count))
	{
		return -EFAULT;
	}
	return count;
}

/*---------------------------------------------------------------------------*/
const struct file_operations oled128x32_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = oled128x32_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = oled128x32_ioctl,
#endif
	.open = oled128x32_open,
	.release = oled128x32_release,
	.llseek		= no_llseek,
	.read		= oled128x32_read,
	.write		= oled128x32_write,
};

/*---------------------------------------------------------------------------*/
static ssize_t
oled128x32_store_pin(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{

	if (!strncmp(buf, "-p", 2)) {
		u32 pin, level;
		buf = buf + 2;
		if((2 == sscanf(buf, "%d %d", &pin, &level)) && pin < 5) {
			mutex_lock(&oled_mutex);
			OledDriver_SetPin(pin, level);
			mutex_unlock(&oled_mutex);
		}
	} else {
		OLED_PRINT("-px 0|1   , x=0 CS; 1 RST ; 2 DC ; 3 SCLK; 4 SDIN , init result = 0x%08X\n" , oled128x32_init_status);
	}

	return count;
}

static inline unsigned char printHex2Number(unsigned char p) 
{
	if( (p>='0') && (p <= '9') )
		return (unsigned char)(p - '0');
	// must UPCASE for speed
	return (unsigned char)(p - 'A' + 10);
}

#if (MICROPANEL_BPP == 1)

	static ssize_t oled128x32_showRaw(struct device *dev, struct device_attribute *attr, char *buf)
	{

		memcpy(buf,oled128x32_ptr->frame_buffer,OLED_FRAMEBUFFER_LENGTH);
		return OLED_FRAMEBUFFER_LENGTH;
	}

	static ssize_t
	oled128x32_storeRaw(struct device *dev, struct device_attribute *attr, const char *bufRaw, size_t count)
	{
	
		int ret = -EINVAL;

		OLED_LOG("oled128x32 count=%d\n",count);
		if( count <= 0) {
			OLED_LOG("oled128x32 count error , ignore it\n");
			return ret;
		}

		if( count > OLED_FRAMEBUFFER_LENGTH) {
			OLED_LOG("oled128x32 count too large , ignore it\n");
			return ret;
		}

		if (unlikely(!bufRaw)) {
			OLED_LOG("oled128x32 buf is invalid\n");
			goto out;
		}
		// copy to framebuffer
		memcpy(oled128x32_ptr->frame_buffer,bufRaw, count);
		{
			ret = 0; // debug
			mutex_lock(&oled_mutex);
			// do any op here
			if(oled128x32_ptr->power_state == 1) {
				Oled_UpdateAll((unsigned char *)oled128x32_ptr->frame_buffer);
				oled128x32_ptr->need_update = 0;
			} else {
				oled128x32_ptr->need_update = 1;
			}
	
			mutex_unlock(&oled_mutex);
		}

		if (ret < 0) {
			OLED_LOG("oled128x32 Message transfer err:%d\n", ret);
		} else {
			ret = count;
		}

	out:
		return ret;
	}

	static ssize_t oled128x32_showString(struct device *dev, struct device_attribute *attr, char *buf)
	{
		int i = 0;
		unsigned char *p = (unsigned char*)oled128x32_ptr->frame_buffer;
		static const unsigned char *toHex = "0123456789ABCDEF";
		for(i = 0 ; i < OLED_FRAMEBUFFER_LENGTH; i++)
		{
			*buf = toHex[((*p) >> 4) & 0xF];
			buf++;
			*buf = toHex[(*p) & 0xF];
			buf++;

			p++;
		}
		*buf=0;
		return (OLED_FRAMEBUFFER_LENGTH*2 + 1);
	}
	static ssize_t
	oled128x32_storeString(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
	{
		unsigned char *pTx;
		unsigned char *bufRaw;
		int i = 0;
		int ret = 0;
		size_t len = count >> 1;
	
		if(len == 0) {
			OLED_LOG("oled128x32 count=%d , ignore it\n",count);
			return count;
		}

		bufRaw = kmalloc(len, GFP_KERNEL);
		if (bufRaw == NULL)
			return -ENOMEM;

		pTx = bufRaw;
		while(i < (len << 1) ) {
			unsigned char data = 0;
			data = printHex2Number((unsigned char)buf[i]);
			data = data << 4;
			i++;
			data = data | printHex2Number((unsigned char)buf[i]);
			i++;
			*pTx = data;
			pTx++;
		}
		ret = oled128x32_storeRaw(dev, attr, (const char *)bufRaw, len);
		kfree(bufRaw);
		if(ret > 0) {
			return count;
		} else {
			return ret;
		}
	
	}
#endif // #if (MICROPANEL_BPP == 1)
static ssize_t
oled128x32_storePower(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned char state;
    mutex_lock(&oled_mutex);
	// do any op here
	state = printHex2Number(buf[0]) == 0 ? 0 : 1;

	if(state == 0)
	{
		Oled_Sleep();
	}
	else
	{
		Oled_WakeUp();
		if(oled128x32_ptr->need_update == 1)
		{
			Oled_UpdateAll((unsigned char *)oled128x32_ptr->frame_buffer);
			oled128x32_ptr->need_update = 0;
		}
	}
	oled128x32_ptr->power_state = state;
	mutex_unlock(&oled_mutex);
	return count;

}
static ssize_t oled128x32_showPower(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d", oled128x32_ptr->power_state);
}

static ssize_t
oled128x32_storeBright(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned char brightness = 0;
	mutex_lock(&oled_mutex);
	// do any op here
	brightness = printHex2Number(buf[0]);
	brightness = brightness << 4;
	brightness = brightness | printHex2Number(buf[1]);
	Oled_Brightness(brightness);
	oled128x32_ptr->brightness = brightness;
	
	mutex_unlock(&oled_mutex);

	return count;

}
static ssize_t
oled128x32_showBright(struct device *dev, struct device_attribute *attr, char *buf)
{

	if(oled128x32_ptr->brightness == -1)
		oled128x32_ptr->brightness = Brightness;

	return snprintf(buf, PAGE_SIZE, "%d", oled128x32_ptr->brightness);

}
#ifdef USE_SPI
	extern int oled_spi_write(const char *bufRaw, size_t count);
	static ssize_t
	oled128x32_storeSpi(struct device *dev, struct device_attribute *attr, const char *bufRaw, size_t count)
	{
	
		int ret = -EINVAL;

		OLED_LOG("oled128x32 storeSpi count=%d\n",count);
		if( count <= 0) {
			OLED_LOG("oled128x32 count error , ignore it\n");
			return 0;
		}


		if (unlikely(!bufRaw)) {
			OLED_LOG("oled128x32 buf is invalid\n");
			goto out;
		}

		{
			ret = 0; // debug
			mutex_lock(&oled_mutex);
			// do any op here
			oled_spi_write(bufRaw,count);
	
			mutex_unlock(&oled_mutex);
		}

		if (ret < 0) {
			OLED_LOG("oled128x32 Message transfer err:%d\n", ret);
		} else {
			ret = count;
		}

	out:
		return ret;
	}
	extern ssize_t __spi_oled128x32_storeCfg(struct device *dev,
			 struct device_attribute *attr, const char *buf, size_t count);
	static ssize_t oled128x32_storeCfg(struct device *dev,
				 struct device_attribute *attr, const char *buf, size_t count)
	{
		return __spi_oled128x32_storeCfg(dev,attr,buf, count);
	}
#endif
static DEVICE_ATTR(pin,      0200, NULL,   oled128x32_store_pin);

#if (MICROPANEL_BPP == 1)
	static DEVICE_ATTR(oled_str, 0600, oled128x32_showString, oled128x32_storeString);
	static DEVICE_ATTR(oled_raw, 0600, oled128x32_showRaw, oled128x32_storeRaw);
#endif
static DEVICE_ATTR(oled_power, 0644, oled128x32_showPower, oled128x32_storePower);
static DEVICE_ATTR(oled_bright, 0644, oled128x32_showBright, oled128x32_storeBright);
#ifdef USE_SPI
	static DEVICE_ATTR(spi_raw, 0200, NULL, oled128x32_storeSpi);
	static DEVICE_ATTR(spi_cfg, 0200, NULL, oled128x32_storeCfg);
#endif
/*---------------------------------------------------------------------------*/
static struct device_attribute *oled128x32_attr_list[] = {
	&dev_attr_pin,
#if (MICROPANEL_BPP == 1)
	&dev_attr_oled_str,
	&dev_attr_oled_raw,
#endif
	&dev_attr_oled_power,
	&dev_attr_oled_bright,
#ifdef USE_SPI
	&dev_attr_spi_raw,
	&dev_attr_spi_cfg,
#endif
};

/*---------------------------------------------------------------------------*/
int oled128x32_create_attr(struct device *dev)
{
	int idx, err = 0;
	int num = (int)(sizeof(oled128x32_attr_list)/sizeof(oled128x32_attr_list[0]));

	if (!dev)
		return -EINVAL;
	for (idx = 0; idx < num; idx++) {
		if (device_create_file(dev, oled128x32_attr_list[idx]))
			break;
	}
	return err;
}
/*---------------------------------------------------------------------------*/
int oled128x32_delete_attr(struct device *dev)
{
	int idx , err = 0;
	int num = (int)(sizeof(oled128x32_attr_list)/sizeof(oled128x32_attr_list[0]));

	if (!dev)
		return -EINVAL;
	for (idx = 0; idx < num; idx++)
		device_remove_file(dev, oled128x32_attr_list[idx]);
	return err;
}

