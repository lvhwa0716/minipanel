
#define LOG_TAG "MicroPanelService_gui"

#include "FontManager.h"
#include "MicroPanelGui.h"
#include "MicroPanelLogo.h"

#include <stdio.h>

static struct MicroPanel_Struct gMicroPanel;

static const unsigned char notmask[8] = {
	0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

#define bDrawLastPoint 1
#define GdClipPoint(psd, x1, y1) mpGui_ClipPoint(x1,y1)
#define GdClipArea(psd, x1, y1, x2, y2) mpGui_ClipArea(x1, y1, x2, y2) 

#if defined(DEBUG_LOG)
	static void mpGui_Print2Console();
	#if defined(HTTP_DEBUG_PORT)
		extern void httpDebug_Init();
		extern void httpDebug_DeInit();
		extern void httpDebug_PushAll(unsigned char * p , int size);
	#else
		#define httpDebug_Init()
		#define httpDebug_DeInit()
		#define httpDebug_PushAll(p, s)
	#endif
#endif

void mpGui_FillRect(int x, int y, int w, int h)
{
	int i;
	for(i = 0 ; i < h ; i++) {
		mpGui_HLine(x, x+w, y + i);
	}

}
void mpGui_Rect(int x, int y, int w, int h)
{
	mpGui_HLine(x, x+w, y);
	mpGui_HLine(x, x+w, y+h);
	mpGui_VLine(x, y, y+h);
	mpGui_VLine(x+w, y, y+h);
}
void mpGui_ClearAll()
{
	memset(gMicroPanel.buffer, 0x0, gMicroPanel.bpl * gMicroPanel.height * sizeof(unsigned char));
}
static inline int mpGui_ClipPoint(int x, int y)
{
	if(( x < 0) || (x >= gMicroPanel.width))
		return 0;

	if(( y < 0) || (y >= gMicroPanel.height))
		return 0;

	return 1;
}

static inline int mpGui_ClipArea(int x1, int y1, int x2, int y2) 
{
	#if (1)
		x1 = x1;
		x2 = x2;
		y1 = y1;
		y2 = y2;
	#else
	// not done
	#endif
	return CLIP_VISIBLE;
}

void mpGui_SetColor(int color)
{
	#if (MICROPANEL_BPP == 1)
		gMicroPanel.color = color == 0 ? 0 : 1;
	#elif (MICROPANEL_BPP == 8)
		gMicroPanel.color = color == 0 ? 0 : 255;
	#endif
}

static inline void mpGui_DrawPixelNoCheck(int x, int y, int c)
{
	#if (MICROPANEL_BPP == 1)
		unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + (x >> 3);
		if(gMicroPanel.opMode == MWROP_XOR)
			*addr ^= c << (7-(x&7));
		else
			*addr = (*addr & notmask[x&7]) | (c << (7-(x&7)));
	#elif (MICROPANEL_BPP == 8)
		unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + x;
		if(gMicroPanel.opMode == MWROP_XOR)
			*addr ^= c;
		else
			*addr = c;
	#endif
}
void mpGui_DrawPixel(int x, int y, int c)
{
	

	if(mpGui_ClipPoint(x, y) == 0)
		return;


	mpGui_DrawPixelNoCheck(x, y, c);
}

int mpGui_ReadPixel(int x, int y)
{
	#if (MICROPANEL_BPP == 1)
		unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + (x >> 3);

		if(mpGui_ClipPoint(x, y) == 0)
			return 0;

		return ( *addr >> (7-(x&7)) ) & 0x01;
	#elif (MICROPANEL_BPP == 8)
		unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + x;
		return	(*addr);
	#endif
}

void mpGui_HLine(int x1, int x2, int y)
{
	
	unsigned char c = (unsigned char)gMicroPanel.color;

	if( ( y < 0 ) || ( y >= gMicroPanel.height ) ) return;

	if(x2 < 0) return;
	if(x2 >= gMicroPanel.width) x2 = gMicroPanel.width - 1;
	if(x1 < 0) x1 = 0;

	DBG_LOG( "%d, %d, %d, %d\n", x1, x2,y, gMicroPanel.bpl );
	#if (MICROPANEL_BPP == 1)
	{
		unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + (x1 >> 3);

		if(gMicroPanel.opMode == MWROP_XOR) {
			while(x1 <= x2) {
				*addr ^= c << (7-(x1&7));
				if((++x1 & 7) == 0)
					++addr;
			}
		} else {
			while(x1 <= x2) {
				*addr = (*addr & notmask[x1&7]) | (c << (7-(x1&7)));
				if((++x1 & 7) == 0)
					++addr;
			}
		}
	}
	#elif (MICROPANEL_BPP == 8)
	{
		unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + x1;
		if(gMicroPanel.opMode == MWROP_XOR) {
			while(x1 <= x2) {
				*addr ^= c;
				++addr;
				x1++;
			}
		} else {
			while(x1 <= x2) {
				*addr = c;
				++addr;
				x1++;
			}
		}
	}
	#endif
}

void mpGui_VLine(int x, int y1, int y2)
{
	int	pitch = gMicroPanel.bpl;
	
	unsigned char c = (unsigned char)gMicroPanel.color;
	if(( x < 0) || (x >= gMicroPanel.width))
		return ;

	if(y2 < 0) return;
	if(y2 >= gMicroPanel.height) y2 = gMicroPanel.height - 1;
	if(y1 < 0) y1 = 0;
	#if (MICROPANEL_BPP == 1)
	{
		unsigned char *addr = gMicroPanel.buffer + y1 * pitch + (x >> 3);
		if(gMicroPanel.opMode == MWROP_XOR)
			while(y1++ <= y2) {
				*addr ^= c << (7-(x&7));
				addr += pitch;
			}
		else
			while(y1++ <= y2) {
				*addr = (*addr & notmask[x&7]) | (c << (7-(x&7)));
				addr += pitch;
			}
	}
	#elif (MICROPANEL_BPP == 8)
	{
		unsigned char *addr = gMicroPanel.buffer + y1 * pitch + (x);
		if(gMicroPanel.opMode == MWROP_XOR)
			while(y1++ <= y2) {
				*addr ^= c;
				addr += pitch;
			}
		else
			while(y1++ <= y2) {
				*addr = c;
				addr += pitch;
			}
	}
	#endif
}
void mpGui_Line(int x1, int y1, int x2, int y2)
{
	int xdelta;		/* width of rectangle around line */
	int ydelta;		/* height of rectangle around line */
	int xinc;		/* increment for moving x coordinate */
	int yinc;		/* increment for moving y coordinate */
	int rem;		/* current remainder */
	

	/* See if the line is horizontal or vertical. If so, then call
	 * special routines.
	 */
	if (y1 == y2) {
		/* call faster line drawing routine */
		mpGui_HLine(x1, x2, y1);
		return;
	}
	if (x1 == x2) {
		/*
		 * Adjust coordinates if not drawing last point.  Tricky.
		 */

		/* call faster line drawing routine */
		mpGui_VLine(x1, y1, y2);
		return;
	}

	/* See if the line is either totally visible or totally invisible. If
	 * so, then the line drawing is easy.
	 */
	switch (GdClipArea(psd, x1, y1, x2, y2)) {
	case CLIP_VISIBLE:
		/*
		 * For size considerations, there's no low-level bresenham
		 * line draw, so we've got to draw all non-vertical
		 * and non-horizontal lines with per-point
		 * clipping for the time being
		 psd->Line(psd, x1, y1, x2, y2, gr_foreground);
		 GdFixCursor(psd);
		 return;
		 */
		break;
	case CLIP_INVISIBLE:
		return;
	}

	/* The line may be partially obscured. Do the draw line algorithm
	 * checking each point against the clipping regions.
	 */
	xdelta = x2 - x1;
	ydelta = y2 - y1;
	if (xdelta < 0)
		xdelta = -xdelta;
	if (ydelta < 0)
		ydelta = -ydelta;
	xinc = (x2 > x1)? 1 : -1;
	yinc = (y2 > y1)? 1 : -1;

	/* draw first point*/
	if (GdClipPoint(psd, x1, y1))
		mpGui_DrawPixelNoCheck( x1, y1, gMicroPanel.color);

	if (xdelta >= ydelta) {
		rem = xdelta / 2;
		for (;;) {
			if (!bDrawLastPoint && x1 == x2)
				break;
			x1 += xinc;
			rem += ydelta;
			if (rem >= xdelta) {
				rem -= xdelta;
				y1 += yinc;
			}


			if (GdClipPoint(psd, x1, y1))
				mpGui_DrawPixelNoCheck( x1, y1, gMicroPanel.color);


			if (bDrawLastPoint && x1 == x2)
				break;

		}
	} else {
		rem = ydelta / 2;
		for (;;) {
			if (!bDrawLastPoint && y1 == y2)
				break;
			y1 += yinc;
			rem += xdelta;
			if (rem >= ydelta) {
				rem -= ydelta;
				x1 += xinc;
			}


			if (GdClipPoint(psd, x1, y1))
				mpGui_DrawPixelNoCheck( x1, y1, gMicroPanel.color);


			if (bDrawLastPoint && y1 == y2)
				break;
		}
	}
}
void mpGui_DrawString(int x, int y, char *String /* only UTF8 */)
{
	FontManager_DrawString(String, x , y);
}
void mpGui_DrawBitmap(int dstx, int dsty, unsigned char* bmp, int bmp_w, int bmp_h, int bmp_pitch , int bmp_bpp)
{
	int		i;
	struct MicroPanel_Struct* dstpsd = &gMicroPanel;
	int srcx = 0;
	int srcy = 0;

	int		dpitch = dstpsd->bpl;
	int		spitch = bmp_pitch;

	int w = bmp_w;
	int h = bmp_h;

	bmp_bpp = bmp_bpp; // not used , must set 1

	if(bmp_bpp != 1)
	{
		DBG_ERR( " bmp_bpp != 1 , ignore\n");
		return;
	}
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

	if( (dstx + w) > dstpsd->width)
	{
		w = dstpsd->width - dstx;
	}

	if( (dsty + h) > dstpsd->height)
	{
		h = dstpsd->height - dsty;
	}

	if((h <= 0) || (w <= 0))
	{
		return;
	}
	#if (MICROPANEL_BPP == 1)
		/* src is LSB 1bpp, dst is LSB 1bpp*/
		unsigned char* dst = ((unsigned char*)dstpsd->buffer) + (dstx>>3) + dsty * dpitch;
		unsigned char* src = ((unsigned char*)bmp) + (srcx>>3) + srcy * spitch;


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
	#elif (MICROPANEL_BPP == 8)
		/* src is LSB 1bpp, dst is LSB 8bpp*/
		unsigned char* dst = ((unsigned char*)dstpsd->buffer) + (dstx) + dsty * dpitch;
		unsigned char* src = ((unsigned char*)bmp) + (srcx>>3) + srcy * spitch;


		while(--h >= 0) {
			unsigned char*	d = dst;
			unsigned char*	s = src;
			int	dx = dstx;
			int	sx = srcx;

			for(i=0; i<w; ++i) {
				*d = (*s >> (7 - (sx&7)) & 0x01) == 0 ? 0 : 0xFF;
				++d;
				if((++sx & 7) == 0)
					++s;
			}
			dst += dpitch;
			src += spitch;
		}
	#endif
}

extern void OledDriver_intfApp_Init(void);
extern void OledDriver_intfApp_DeInit(void);
extern void OledDriver_intfApp_Sleep(void);
extern void OledDriver_intfApp_WakeUp(void);
extern void OledDriver_intfApp_Brightness(int b);
extern void OledDriver_intfApp_Update(unsigned char *pBuf, int x, int y, int w, int h);

void mpGui_Init(void)
{

	httpDebug_Init();
	// Software init
	gMicroPanel.width = MICROPANEL_WIDTH;
	gMicroPanel.height = MICROPANEL_HEIGHT;
	gMicroPanel.bpp = MICROPANEL_BPP;
	#if (MICROPANEL_BPP == 1)
		gMicroPanel.bpl = (MICROPANEL_WIDTH + 7) / 8;
	#elif (MICROPANEL_BPP == 8)
		gMicroPanel.bpl = MICROPANEL_WIDTH;
	#endif
	gMicroPanel.power_status = MICROPANEL_POWER_OFF;
	gMicroPanel.buffer = (unsigned char*)calloc(gMicroPanel.bpl * gMicroPanel.height, sizeof(unsigned char));
	gMicroPanel.opMode = MWROP_COPY;
	gMicroPanel.color = 0;

	// init FontManager
	//FontManager_Init(MICROPANEL_WIDTH, MICROPANEL_HEIGHT, MICROPANEL_BPP, gMicroPanel.bpl, gMicroPanel.buffer,FontManager_ShowCallback_Sample);
	FontManager_Init(MICROPANEL_WIDTH, MICROPANEL_HEIGHT, MICROPANEL_BPP, gMicroPanel.bpl, gMicroPanel.buffer,NULL);

	OledDriver_intfApp_Init();

	// Draw Logo
	mpGui_DrawBitmap(0, 0, (unsigned char*)IMG_logo.pData, IMG_logo.width, IMG_logo.height, IMG_logo.bpl , 1);
	mpGui_UpdateScreen(0,0,MICROPANEL_WIDTH, MICROPANEL_HEIGHT);
}

void mpGui_DeInit(void)
{
	// Power Down
	// destroy FontManager
	FontManager_DeInit();
	free(gMicroPanel.buffer);
	OledDriver_intfApp_DeInit();

	httpDebug_DeInit();
}

void mpGui_Sleep(int level)
{
	level = level;
	OledDriver_intfApp_Sleep();
}
void mpGui_Wakeup(void)
{
	OledDriver_intfApp_WakeUp();
}

void mpGui_Brightness(int b)
{
	OledDriver_intfApp_Brightness(b);
}
void mpGui_UpdateScreen(int x, int y, int w, int h)
{

	if( x < 0 ) {
		w = x + w;
		x = 0;
	}
	if( y < 0 ) {
		h = y + w;
		y = 0;
	}

	if( (w + x) >= MICROPANEL_WIDTH ) {
		w = MICROPANEL_WIDTH - x - 1;
	}

	if( (h + y) >= MICROPANEL_HEIGHT ) {
		h = MICROPANEL_HEIGHT - y - 1;
	}

	if( ( w <= 0 ) || (h <= 0) ) {
		return;
	}

	OledDriver_intfApp_Update(gMicroPanel.buffer, x, y, w, h);
	#if defined(DEBUG_LOG)
			#if defined(HTTP_DEBUG_PORT)
				// post to host 
				httpDebug_PushAll(gMicroPanel.buffer , gMicroPanel.bpl * gMicroPanel.height * sizeof(unsigned char));
			#else
				mpGui_Print2Console(); // debug
			#endif
	#endif
}

void mpGui_FontSize(int width, int height)
{
	FontManager_FontSize(width, height);
}


static void mpGui_Print2Console(void)
{

	int  i, j;

	unsigned char *image = gMicroPanel.buffer;
	#if (MICROPANEL_BPP == 1)
		for ( i = 0; i < gMicroPanel.height; i++ )
		{
			for ( j = 0; j < gMicroPanel.width; j++ ) {
				unsigned char *addr = image + i * gMicroPanel.bpl + (j >> 3);
				putchar( ( *addr >> (7-(j&7)) ) & 0x01 ? '*' : ' ');
			}
			putchar( '\n' );
		}
		putchar( '\n' );

	#elif (MICROPANEL_BPP == 8)
		for ( i = 0; i < gMicroPanel.height; i++ )
		{
			for ( j = 0; j < gMicroPanel.width; j++ ) {
				unsigned char *addr = image + i * gMicroPanel.bpl + j;
				putchar( ( *addr) & 0x80 ? '*' : ' ');
			}
			putchar( '\n' );
		}
		putchar( '\n' );
	#endif

}
