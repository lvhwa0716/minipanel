#ifndef __MICROPANELGUI_H__
#define __MICROPANELGUI_H__

#include <stdio.h>
#include <unistd.h>

// most code copy from microwin

#define MICROPANEL_WIDTH	128
#define MICROPANEL_HEIGHT	32
#define MICROPANEL_BPP		1	// only support BPP = 1

#define WHITE				1
#define BLACK				0

#define MWROP_COPY			0
#define MWROP_XOR			1

#define CLIP_VISIBLE		0
#define CLIP_INVISIBLE		1
#define CLIP_PARTIAL		2

#define MICROPANEL_POWER_OFF	0
#define MICROPANEL_POWER_SLEEP	1
#define MICROPANEL_POWER_ACTIVE	2

#ifdef DEBUG_LOG
	#ifdef ANDROID
		#include <utils/Log.h>
		#define DBG_LOG(fmt,arg...) ALOGD(fmt, ##arg)
		#define DBG_ERR(fmt,arg...) ALOGE(fmt, ##arg)
	#else
		#define DBG_LOG(fmt,arg...) fprintf ( stdout, fmt, ##arg)
		#define DBG_ERR(fmt,arg...) fprintf ( stderr, fmt, ##arg)
	#endif
#else
	#define DBG_LOG(fmt,arg...)
	#ifdef ANDROID
		#include <utils/Log.h>
		#define DBG_ERR(fmt,arg...) ALOGE(fmt, ##arg)
	#else
		#define DBG_ERR(fmt,arg...) fprintf ( stderr, fmt, ##arg)
	#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif

struct MicroPanel_Struct {
	int width;
	int height;
	int bpp;
	int bpl;
	int power_status; // 0 = power off , 1= sleep , 2 = active
	int color;
	int opMode;
	// Hardware handle
	int lcdHandle;

	unsigned char *buffer;
};

struct __ImageStruct__ {
	int width;
	int height;
	int bpl;
	const unsigned char *pData;
};

#ifndef byte
	#define byte unsigned char
#endif
void mpGui_Rect(int x, int y, int w, int h);
void mpGui_FillRect(int x, int y, int w, int h);
void mpGui_ClearAll();
void mpGui_SetColor(int color);
void mpGui_DrawPixel(int x, int y, int color);
int mpGui_ReadPixel(int x, int y);
void mpGui_HLine(int x1, int x2, int y); // x1 < x2
void mpGui_VLine(int x, int y1, int y2); // y1 < y2
void mpGui_Line(int x1, int y1, int x2, int y2);
void mpGui_DrawString(int x, int y, char *String /* only UTF8 */);
void mpGui_DrawBitmap(int x, int y, unsigned char* bmp, int bmp_w, int bmp_h, int bmp_pitch , int bmp_bpp);
void mpGui_FontSize(int width, int height); 

void mpGui_Init(void);
void mpGui_DeInit(void);
void mpGui_Sleep(int level);
void mpGui_Wakeup(void);
void mpGui_Brightness(int b);
void mpGui_UpdateScreen(int x, int y, int w, int h);

#if defined(__cplusplus)
}
#endif

#endif // __MICROPANELGUI_H__
