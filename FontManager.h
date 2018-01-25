
#ifndef __FONTMANAGER_H__
#define __FONTMANAGER_H__

#include <stdio.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define DEBUG_FONTMANAGER	1

#define gFontSize 2

typedef void (*FontManager_ShowCallbak)(unsigned char *bitmap, int w, int h, int bpl, int bpp);

struct FontManager_Struct {
	FT_Library    library;
	FT_Face faces[gFontSize];
	FT_UInt  fontWidth;
	FT_UInt  fontHeight;
	FT_Matrix default_matrix;
	int width;
	int height;
	int bpp;
	int bpl;
	int load_flags;
	int direct2framebuffer;
	unsigned char *bitmap;
	FontManager_ShowCallbak show;
};

// when framebuffer == NULL, must give FontManager_ShowCallbak, or ignore it, or nothing to show
int FontManager_Init(int screen_w, int screen_h, int bpp , unsigned char* framebuffer,FontManager_ShowCallbak cb);
void FontManager_DrawString(char* text, int x, int y);
void FontManager_ClearBitmap(void);
void FontManager_DeInit(void);
void FontManager_Show(void); 

void FontManager_ShowCallback_Sample(unsigned char *bitmap, int w, int h, int bpl, int bpp);
#endif // __FONTMANAGER_H__
