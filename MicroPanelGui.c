
#define LOG_TAG "MicroPanelService_gui"

#include "FontManager.h"
#include "MicroPanelGui.h"
#include "MicroPanelLogo.h"

#include <stdio.h>

// must undefine it when release
#define HTTP_DEBUG_PORT	9234

#if defined(HTTP_DEBUG_PORT)
	#include <curl/curl.h>
#endif

static struct MicroPanel_Struct gMicroPanel;

static const unsigned char notmask[8] = {
	0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

#define bDrawLastPoint 1
#define GdClipPoint(psd, x1, y1) mpGui_ClipPoint(x1,y1)
#define GdClipArea(psd, x1, y1, x2, y2) mpGui_ClipArea(x1, y1, x2, y2) 

#if defined(DEBUG_LOG)
	static void mpGui_Print2Console();
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
	gMicroPanel.color = color;
}

static inline void mpGui_DrawPixelNoCheck(int x, int y, int c)
{
	unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + (x >> 3);
	if(gMicroPanel.opMode == MWROP_XOR)
		*addr ^= c << (7-(x&7));
	else
		*addr = (*addr & notmask[x&7]) | (c << (7-(x&7)));
}
void mpGui_DrawPixel(int x, int y, int c)
{
	

	if(mpGui_ClipPoint(x, y) == 0)
		return;


	mpGui_DrawPixelNoCheck(x, y, c);
}

int mpGui_ReadPixel(int x, int y)
{
	unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + (x >> 3);

	if(mpGui_ClipPoint(x, y) == 0)
		return 0;

	return ( *addr >> (7-(x&7)) ) & 0x01;

}

void mpGui_HLine(int x1, int x2, int y)
{
	unsigned char *addr = gMicroPanel.buffer + y * gMicroPanel.bpl + (x1 >> 3);
	unsigned char c = (unsigned char)gMicroPanel.color;

	if( ( y < 0 ) || ( y >= gMicroPanel.height ) ) return;

	if(x2 < 0) return;
	if(x2 >= gMicroPanel.width) x2 = gMicroPanel.width - 1;
	if(x1 < 0) x1 = 0;

	DBG_LOG( "%d, %d, %d, %d\n", x1, x2,y, gMicroPanel.bpl );

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

void mpGui_VLine(int x, int y1, int y2)
{
	int	pitch = gMicroPanel.bpl;
	unsigned char *addr = gMicroPanel.buffer + y1 * pitch + (x >> 3);
	unsigned char c = (unsigned char)gMicroPanel.color;
	if(( x < 0) || (x >= gMicroPanel.width))
		return ;

	if(y2 < 0) return;
	if(y2 >= gMicroPanel.height) y2 = gMicroPanel.height - 1;
	if(y1 < 0) y1 = 0;

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
}

extern void OledDriver_intfApp_Init(void);
extern void OledDriver_intfApp_DeInit(void);
extern void OledDriver_intfApp_Sleep(void);
extern void OledDriver_intfApp_WakeUp(void);
extern void OledDriver_intfApp_Brightness(int b);
extern void OledDriver_intfApp_Update(unsigned char *pBuf, int x, int y, int w, int h);

void mpGui_Init(void)
{

	#if defined(HTTP_DEBUG_PORT)
		curl_global_init(CURL_GLOBAL_ALL);  
	#endif
	// Software init
	gMicroPanel.width = MICROPANEL_WIDTH;
	gMicroPanel.height = MICROPANEL_HEIGHT;
	gMicroPanel.bpp = MICROPANEL_BPP;
	gMicroPanel.bpl = (MICROPANEL_WIDTH + 7) / 8;
	gMicroPanel.power_status = MICROPANEL_POWER_OFF;
	gMicroPanel.buffer = (unsigned char*)calloc(gMicroPanel.bpl * gMicroPanel.height, sizeof(unsigned char));
	gMicroPanel.opMode = MWROP_COPY;

	// init FontManager
	FontManager_Init(MICROPANEL_WIDTH, MICROPANEL_HEIGHT, 1, gMicroPanel.bpl, gMicroPanel.buffer,FontManager_ShowCallback_Sample);

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

	#if defined(HTTP_DEBUG_PORT)
		curl_global_cleanup();
	#endif
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
		mpGui_Print2Console(); // debug
	#endif
}

void mpGui_FontSize(int width, int height)
{
	FontManager_FontSize(width, height);
}
#if defined(HTTP_DEBUG_PORT)
	struct _update_context_ {
		unsigned char *data;
		int size;
		int pos;

	} g_upload_ctx;

	static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)  
	{  
		struct _update_context_ *ctx = (struct _update_context_ *) stream;  
		size_t len = 0;

		if (ctx->pos >= ctx->size) {
			return 0;
		}

		if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
			return 0;
		}

		len = ctx->size - ctx->pos;
		if (len > size*nmemb) {
			len = size * nmemb;
		}

		memcpy(ptr, ctx->data + ctx->pos, len);
		ctx->pos += len;
		DBG_LOG("send len=%d", len);
		return len;
	}  
#endif

static void mpGui_Print2Console(void) {
	#if defined(HTTP_DEBUG_PORT)
		// post to host 
		CURL *curl = curl_easy_init();
		CURLcode res;
		
		if (curl) {
			char url[255];
			sprintf(url, "http://localhost:%d", HTTP_DEBUG_PORT);
			DBG_LOG("server : %s" , url);

			g_upload_ctx.data = gMicroPanel.buffer;
			g_upload_ctx.pos = 0;
			g_upload_ctx.size = gMicroPanel.bpl * gMicroPanel.height * sizeof(unsigned char);

			curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl, CURLOPT_PUT, 1L);
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_READDATA, &g_upload_ctx);
			curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)(g_upload_ctx.size));

			//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

			res = curl_easy_perform(curl);
			if (res != CURLE_OK)
				DBG_ERR( "curl_easy_perform() failed: %s\n",  curl_easy_strerror(res));

			curl_easy_cleanup(curl);
		}
	#else
		
		int  i, j;

		unsigned char *image = gMicroPanel.buffer;

		if(gMicroPanel.bpp == 1)
		{
			for ( i = 0; i < gMicroPanel.height; i++ )
			{
				for ( j = 0; j < gMicroPanel.width; j++ ) {
					unsigned char *addr = image + i * gMicroPanel.bpl + (j >> 3);
					putchar( ( *addr >> (7-(j&7)) ) & 0x01 ? '*' : ' ');
				}
				putchar( '\n' );
			}
			putchar( '\n' );
		}

	#endif
}
