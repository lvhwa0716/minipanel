

#include "OledMono.h"
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

int oled128x32_init_status = 0;

extern void OledDriver_SetPin(int pin, int level);

static void drv_bitblt(short dstx, short dsty, 
		unsigned char* bmp, unsigned short bmp_w, unsigned short bmp_h, 
		unsigned short bmp_pitch , int bmp_bpp)
{
	static const unsigned char notmask[8] = {
		0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
	int		i;
	int srcx = 0;
	int srcy = 0;
	
	int		dpitch = 128 / 8;
	int		spitch = bmp_pitch;

	int 	dst_w = 128;
	int 	dst_h = 32;

	int w = bmp_w;
	int h = bmp_h;
	unsigned char* dst;
	unsigned char* src;

	bmp_bpp = bmp_bpp; // not used , must set 1
	// need clip
	if(dstx < 0)
	{
		srcx = -dstx;
		w = w + dstx;
		dstx = 0;

	}
	if(dsty < 0)
	{
		srcy = -dsty;
		h = h + dsty;
		dsty = 0;
	}

	if( (dstx + w) > dst_w)
	{
		w = dst_w - dstx;
	}

	if( (dsty + h) > dst_h)
	{
		h = dst_h - dsty;
	}

	if((h <= 0) || (w <= 0))
	{
		return;
	}

	/* src is LSB 1bpp, dst is LSB 1bpp*/
	dst = ((unsigned char*)oled128x32_ptr->frame_buffer) + (dstx>>3) + dsty * dpitch;
	src = ((unsigned char*)bmp) + (srcx>>3) + srcy * spitch;


	while(--h >= 0) {
		unsigned char*	d = dst;
		unsigned char*	s = src;
		int	dx = dstx;
		int	sx = srcx;

		for(i=0; i<w; ++i) {
			*d = (*d & notmask[dx&7]) | ((*s >> (7 - (sx&7)) & 0x01) << (7 - (dx&7)));
			if((++dx & 7) == 0)
				++d;
			if((++sx & 7) == 0)
				++s;
		}
		dst += dpitch;
		src += spitch;
	}
}

#ifdef USE_GPIO
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
	struct oled128x32_obj_t *obj = oled128x32_ptr;

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

	long res;


	switch (cmd) {
	case 0:
		{
			res = 0;
			break;
		}
	
	default:
		{
			res = -EPERM;
			break;
		}
	}

	if (res == -EACCES)
		OLED_LOG(" cmd = 0x%8X, invalid pointer\n", cmd);
	else if (res < 0)
		OLED_LOG(" cmd = 0x%8X, err = %ld\n", cmd, res);
	return res;
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
};

#endif // USE_GPIO
/*---------------------------------------------------------------------------*/
static ssize_t
oled128x32_store_pin(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{

	if (!strncmp(buf, "-p", 2)) {
		u32 pin, level;
		buf = buf + 2;
		if((2 == sscanf(buf, "%d %d", &pin, &level)) && pin < 5) {
			
			{

				int rt = down_interruptible(&oled128x32_ptr->driver_lock);
				if (rt < 0) {
					up(&oled128x32_ptr->driver_lock);
					OLED_PRINT("Lock Error\n");
					return count;
				}
			}
			OledDriver_SetPin(pin, level);
			up(&oled128x32_ptr->driver_lock);
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

static ssize_t oled128x32_showRaw(struct device *dev, struct device_attribute *attr, char *buf)
{

	memcpy(buf,oled128x32_ptr->frame_buffer,OLED_FRAMEBUFFER_LENGTH);
	return OLED_FRAMEBUFFER_LENGTH;
}

static ssize_t
oled128x32_storeRaw(struct device *dev, struct device_attribute *attr, const char *bufRaw, size_t count)
{
	
	int ret = -EINVAL;
	int rt = 0;
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
		rt = down_interruptible(&oled128x32_ptr->driver_lock);
		if (rt < 0) {
			up(&oled128x32_ptr->driver_lock);
			return count;
		}
		// do any op here
		if(oled128x32_ptr->power_state == 1) {
			Oled_UpdateAll((unsigned char *)oled128x32_ptr->frame_buffer);
			oled128x32_ptr->need_update = 0;
		} else {
			oled128x32_ptr->need_update = 1;
		}
	
		up(&oled128x32_ptr->driver_lock);
	}

	if (ret < 0) {
		OLED_LOG("oled128x32 Message transfer err:%d\n", ret);
	} else {
		ret = count;
	}

out:
	return ret;
}

static ssize_t
oled128x32_storeRawRect(struct device *dev, struct device_attribute *attr, const char *bufRaw, size_t count)
{
	struct oled128x32_rectUpdate *pRect = (struct oled128x32_rectUpdate *)bufRaw;
	if(count < oled128x32_rectUpdate_headSize) 
	{
		OLED_PRINT("buffer to small%d\n",count);
		return -EINVAL;
	}

	OLED_LOG("RawRect x=%u,y=%u,w=%u,h=%u,bpl=%u,size=%u\n",pRect->x,pRect->y,pRect->w,pRect->h,pRect->bpl,pRect->size);

	if( (pRect->size + oled128x32_rectUpdate_headSize) != count) 
	{
		OLED_PRINT("buffer size error \n");
		return -EINVAL;
	}
	
	if( pRect->size != (pRect->bpl * pRect->h)) 
	{
		OLED_PRINT("buffer size error : size != (bpl * h)\n");
		return -EINVAL;
	}

	// TODO , bitblt rect to oled128x32_ptr->frame_buffer
	drv_bitblt(pRect->x, pRect->y, pRect->frame_buffer, pRect->w, pRect->h, pRect->bpl , 1);
	// TODO , update rect oled128x32_ptr->frame_buffer to oled
	{

		int rt = down_interruptible(&oled128x32_ptr->driver_lock);
		if (rt < 0) {
			up(&oled128x32_ptr->driver_lock);
			return count;
		}
		// do any op here
		if(oled128x32_ptr->power_state == 1) {
			short x1,y1,x2,y2;
			x1 = pRect->x;
			y1 = pRect->y;

			x2 = pRect->x + (short)pRect->w;
			y2 = pRect->y + (short)pRect->h;
			if(pRect->x < 0)
			{
				x1 = 0;
			}
			if(pRect->y < 0)
			{
				y1 = 0;
			}


			if((x2 > 0) && (y2 > 0))
			{
				Oled_UpdateRect(x1,y1,x2,y2,(unsigned char *)oled128x32_ptr->frame_buffer);
				oled128x32_ptr->need_update = 0;
			}
			
		} else {
			oled128x32_ptr->need_update = 1;
		}
	
		up(&oled128x32_ptr->driver_lock);
	}
	return count;
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
static ssize_t
oled128x32_storePower(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned char state;
    int rt = down_interruptible(&oled128x32_ptr->driver_lock);
    if (rt < 0) {
		up(&oled128x32_ptr->driver_lock);
		return count;
    }
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
	up(&oled128x32_ptr->driver_lock);
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
    int rt = down_interruptible(&oled128x32_ptr->driver_lock);
    if (rt < 0) {
		up(&oled128x32_ptr->driver_lock);
		return count;
    }
	// do any op here
	brightness = printHex2Number(buf[0]);
	brightness = brightness << 4;
	brightness = brightness | printHex2Number(buf[1]);
	Oled_Brightness(brightness);
	oled128x32_ptr->brightness = brightness;
	
	up(&oled128x32_ptr->driver_lock);

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
		int rt = 0;
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
			rt = down_interruptible(&oled128x32_ptr->driver_lock);
			if (rt < 0) {
				up(&oled128x32_ptr->driver_lock);
				return count;
			}
			// do any op here
			oled_spi_write(bufRaw,count);
	
			up(&oled128x32_ptr->driver_lock);
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
static DEVICE_ATTR(oled_str, 0600, oled128x32_showString, oled128x32_storeString);
static DEVICE_ATTR(oled_raw, 0600, oled128x32_showRaw, oled128x32_storeRaw);
static DEVICE_ATTR(oled_rawrect, 0200, NULL, oled128x32_storeRawRect);
static DEVICE_ATTR(oled_power, 0644, oled128x32_showPower, oled128x32_storePower);
static DEVICE_ATTR(oled_bright, 0644, oled128x32_showBright, oled128x32_storeBright);
#ifdef USE_SPI
	static DEVICE_ATTR(spi_raw, 0200, NULL, oled128x32_storeSpi);
	static DEVICE_ATTR(spi_cfg, 0200, NULL, oled128x32_storeCfg);
#endif
/*---------------------------------------------------------------------------*/
static struct device_attribute *oled128x32_attr_list[] = {
	&dev_attr_pin,
	&dev_attr_oled_str,
	&dev_attr_oled_raw,
	&dev_attr_oled_rawrect,
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

