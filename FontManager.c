
#define LOG_TAG "MicroPanelService_font"

#include "FontManager.h"
#include "MicroPanelGui.h"
#include "math.h"

static const char* gFontFile[gFontSize] = {
	#ifdef ANDROID
		"/system/fonts/Roboto-Regular.ttf", // English from Android
		"/system/fonts/NotoSansSC-Regular.otf" // SimpleChinese for Android
	#else
		"/home/lvh/freetype/Padauk.ttf", // English from Android
		"/home/lvh/freetype/NotoSansSC-Regular.otf" // SimpleChinese for Android
	#endif
	
};


static struct FontManager_Struct gFontManager;

static void FontManager_ShowCallback_Dummy(unsigned char *bitmap, int w, int h, int bpl, int bpp)
{
	// avoid compiler warning
	bitmap = bitmap;
	w = w;
	h = h;
	bpl = bpl;
	bpp = bpp;
}

int FontManager_Init(int screen_w, int screen_h, int bpp, int bpl, unsigned char* framebuffer, FontManager_ShowCallbak cb) 
{
	gFontManager.fontWidth = 16;
	gFontManager.fontHeight = 16;
	gFontManager.width = screen_w;
	gFontManager.height = screen_h;
	gFontManager.bpp = bpp;
	gFontManager.load_flags = FT_LOAD_RENDER | FT_LOAD_DEFAULT;
	gFontManager.bpl = bpl;

	if(bpp == 1)
	{
		gFontManager.load_flags |= FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO;
	}
	else if(bpp == 8)
	{
		
	}
	else
	{
		DBG_ERR( "BPP not support , must 1 or 8 \n" );

		return -1;
	}
	int error = FT_Init_FreeType( &gFontManager.library ); 
	int i;
	for( i = 0; i < gFontSize ; i++)
	{
		error = FT_New_Face( gFontManager.library, gFontFile[i], 0, &gFontManager.faces[i] );
		
		error = FT_Select_Charmap( gFontManager.faces[i], FT_ENCODING_UNICODE );
		error = FT_Set_Pixel_Sizes(gFontManager.faces[i], gFontManager.fontWidth, gFontManager.fontHeight);
	}
	double angle = ( 0.0 / 360 ) * 3.14159 * 2;      /* use 25 degrees     */
	/* set up matrix */
	gFontManager.default_matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	gFontManager.default_matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	gFontManager.default_matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	gFontManager.default_matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );
	if(framebuffer == NULL)
	{
		gFontManager.direct2framebuffer = 0;
		gFontManager.bitmap = (unsigned char*)calloc(gFontManager.bpl * gFontManager.height, sizeof(unsigned char));
		if(gFontManager.bitmap == NULL)
		{
			DBG_ERR( "allocate framebuffer error" );

			return -2;
		}
	}
	else
	{
		gFontManager.direct2framebuffer = 1;
		gFontManager.bitmap = framebuffer;
	}

	if( cb != NULL) 
	{
		gFontManager.show = cb;
	}
	else
	{
		gFontManager.show = FontManager_ShowCallback_Dummy;
	}
	return error;
}


static int FontManager_utf8Next( const char**  pcursor, const char*   end )
{
	const unsigned char*  p = (const unsigned char*)*pcursor;
	int                   ch;


	if ( (const char*)p >= end ) /* end of stream */
		return -1;

	ch = *p++;
	if ( ch >= 0x80 )
	{
		int  len;


		if ( ch < 0xc0 )  /* malformed data */
			goto BAD_DATA;
		else if ( ch < 0xe0 )
		{
			len = 1;
			ch &= 0x1f;
		}
		else if ( ch < 0xf0 )
		{
			len = 2;
			ch &= 0x0f;
		}
		else
		{
			len = 3;
			ch &= 0x07;
		}

		while ( len > 0 )
		{
			if ( (const char*)p >= end || ( p[0] & 0xc0 ) != 0x80 )
				goto BAD_DATA;

			ch   = ( ch << 6 ) | ( p[0] & 0x3f );
			p   += 1;
			len -= 1;
		}
	}

	*pcursor = (const char*)p;

	return ch;

	BAD_DATA:
	return -1;
}

static const unsigned char notmask[8] = {
	0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

#define ADDR8 unsigned char*
#define MWCOORD int
static inline void FontManager_1_blitfromFont( MWCOORD dstx, MWCOORD dsty,FT_Bitmap* srcpsd)
{
	// from MicroWindow fblib1rev.c
	int		i;
	struct FontManager_Struct* dstpsd = &gFontManager;
	MWCOORD srcx = 0;
	MWCOORD srcy = 0;

	int		dpitch = dstpsd->bpl;
	int		spitch = srcpsd->pitch;

	MWCOORD w = srcpsd->width;
	MWCOORD h = srcpsd->rows;

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

	DBG_LOG( "%d,%d,%d,%d\n", srcx, srcy,dstx,dsty);


	if((h <= 0) || (w <= 0))
	{
		return;
	}

	/* src is LSB 1bpp, dst is LSB 1bpp*/
	ADDR8 dst = ((ADDR8)dstpsd->bitmap) + (dstx>>3) + dsty * dpitch;
	ADDR8 src = ((ADDR8)srcpsd->buffer) + (srcx>>3) + srcy * spitch;


	while(--h >= 0) {
		ADDR8	d = dst;
		ADDR8	s = src;
		MWCOORD	dx = dstx;
		MWCOORD	sx = srcx;

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
#undef ADDR8
#undef MWCOORD
static void FontManager_FillBitmap( FT_Bitmap*  bitmap, FT_Int x, FT_Int y)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;
	unsigned char *image = gFontManager.bitmap;
	int BPL = gFontManager.bpl;

	if(gFontManager.bpp == 1)
	{
		#if (0)
			for ( i = x, p = 0; i < x_max; i++, p++ )
			{
				for ( j = y, q = 0; j < y_max; j++, q++ )
				{
					if ( i < 0 || j < 0 || i >= gFontManager.width || j >= gFontManager.height )
						continue;
					unsigned char *addr = image + j * BPL + (i >> 3);
					unsigned char c = ( bitmap->buffer[q * bitmap->pitch + (p >> 3)] >> (7-(p&7)) ) & 0x01;
					*addr = (*addr & notmask[i&7]) | (c << (7-(i&7)));
				}
			}
		#else
			FontManager_1_blitfromFont( x, y, bitmap);
		#endif
	}
	else if(gFontManager.bpp == 8)
	{
		for ( i = x, p = 0; i < x_max; i++, p++ )
		{
			for ( j = y, q = 0; j < y_max; j++, q++ )
			{
				if ( i < 0 || j < 0 || i >= gFontManager.width || j >= gFontManager.height )
					continue;
					image[BPL * j + i] |= bitmap->buffer[q * bitmap->pitch + p];
			}
		}
	}

}

void FontManager_DrawString(char* text, int x, int y) 
{
	FT_GlyphSlot  slot;
	FT_Matrix     matrix;                 /* transformation matrix */
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Error      error;

	memcpy(&matrix , &gFontManager.default_matrix, sizeof(FT_Matrix));
	
	pen.x = 0;
	pen.y = 0;


	const char*  p;
	const char*  pEnd;
	p    = text;
	int          ch;
	pEnd = p + strlen( text );


	do{
		ch = FontManager_utf8Next( &p, pEnd );
		if ( ch < 0 )
		{
			break;
		}

		int face_index = 0;
		for( ; face_index < gFontSize ; face_index++)
		{
			FT_UInt glyph_index = (FT_UInt)ch;
			if ( gFontManager.faces[face_index]->charmap )
				glyph_index = FT_Get_Char_Index( gFontManager.faces[face_index], ch );

			DBG_LOG( "debug glyph_index: %d\n", glyph_index );

			if(glyph_index != 0) {
				slot = gFontManager.faces[face_index]->glyph;
				FT_Set_Transform( gFontManager.faces[face_index], NULL /*&matrix*/, &pen );
				error = FT_Load_Glyph( gFontManager.faces[face_index], glyph_index, gFontManager.load_flags );
				break;
			}
			else
				error = -2;
		}

		if ( error )
			continue;                 /* ignore errors */


		/* now, draw to our target surface (convert position) */
		FontManager_FillBitmap( &slot->bitmap,
				 slot->bitmap_left + x,
				 gFontManager.fontHeight - slot->bitmap_top + y);

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;

	} while(1);
}

void FontManager_ClearBitmap(void)
{
	if(gFontManager.direct2framebuffer == 0)
	{
		memset(gFontManager.bitmap, 0, sizeof(unsigned char) * gFontManager.bpl * gFontManager.height);
	}
}


void FontManager_Show(void)
{

	gFontManager.show(gFontManager.bitmap,gFontManager.width,gFontManager.height,gFontManager.bpl,gFontManager.bpp);


}
void FontManager_DeInit(void) 
{
	int i;
	for( i = 0; i < gFontSize ; i++)
	{
		FT_Done_Face    ( gFontManager.faces[i] );
	}


	if(gFontManager.direct2framebuffer == 0)
	{
		free(gFontManager.bitmap);
	}

	FT_Done_FreeType( gFontManager.library );

}
void FontManager_ShowCallback_Sample(unsigned char *bitmap, int w, int h, int bpl, int bpp)
{
	int  i, j;

	unsigned char *image = bitmap;

	DBG_LOG("Font Callback %d,%d,%d,%d\n", w, h, bpl, bpp );

	if(bpp == 1)
	{
		for ( i = 0; i < h; i++ )
		{
			for ( j = 0; j < w; j++ ) {
				unsigned char *addr = image + i * bpl + (j >> 3);
				putchar( ( *addr >> (7-(j&7)) ) & 0x01 ? '*' : ' ');
			}
			putchar( '\n' );
		}
	}
	else if(bpp == 8)
	{
		for ( i = 0; i < h; i++ )
		{
			for ( j = 0; j < w; j++ ) {
				if(image[i*bpl + j] == 0) {
					putchar(' ');
				} else if(image[i*bpl + j] < 128) {
					putchar('+');
				} else {
					putchar('*');
				}
			}
			putchar( '\n' );
		}
	}
}
void FontManager_FontSize(int width, int height)
{
	int error;
	int i;
	gFontManager.fontWidth = width;
	gFontManager.fontHeight = height;

	for( i = 0; i < gFontSize ; i++)
	{
		
		error = FT_Set_Pixel_Sizes(gFontManager.faces[i], gFontManager.fontWidth, gFontManager.fontHeight);
		if(error != 0)
		{
			DBG_ERR( "Change FontSize error w=%d, h=%d", width, height);
		}
	}
}

