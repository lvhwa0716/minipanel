#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "OledMonoIoctl.h"
#include "OledDriverTest.h"
/***************************************************************************************/

#define OLED_DEVICE "/dev/oled128x32"


void usage()
{
	fprintf ( stderr, "usage: white | black | image | reset\n" );

}

unsigned char frame_buffer[OLED_FRAMEBUFFER_SIZE];
struct image_local {
	const char *name;
	const unsigned char* pImage;
} g_Image[] = {
	{"init",init_Img},
	{"logo",tcl_logo},
	{"",NULL}
};

static const unsigned char dst_mask_[8] = {0x01, 0x02 , 0x04 ,0x08,0x10 ,0x20,0x40,0x80};
static const unsigned char src_mask_[8] = { 0x80,0x40, 0x20 ,0x10 ,0x08,0x04,0x02,0x01 };

void FrameBufferDump(unsigned char *bitmap, int w, int h)
{
	int y, x;

	for(x = 0 ; x < w * h / 8 ; x++) {
		if( x % 16 == 0) 	printf ( "\n");
		printf("0x%02X, " , bitmap[x]);
	}


	printf ( "\n\n   BITMAP    \n\n");

	for( y = 0; y < h  ; y++ ) {
        for( x = 0; x < w ; x++ ) {
			int offset = x + (y / 8 ) * 128;
			putchar( bitmap[ offset]  & dst_mask_[y % 8] ? '*' : ' ');
		}
		putchar( '\n' );
	}
}

#define getpixel(buf, x, y) (buf[((y) << 4) + ((x) >> 3)] & src_mask_[(x) & 0x7])
#define setpixel(buf, x, y) (buf[(x) + ((y) >> 3 ) * 128] |= dst_mask_[(y) & 0x7])
static unsigned char * __Oled_Convert(unsigned char *pBuf)
{
	int y, x;
	
	static unsigned char DRAM_V_FB[128 * 32 / 8];
	memset(DRAM_V_FB, 0, sizeof(DRAM_V_FB));
	for( y = 0; y < 32 ; y++ ) {
        for( x = 0; x < 128  ; x++ ) {
			
            if( 0 != getpixel(pBuf, x , y) )
            	setpixel(DRAM_V_FB, x , y);
		}
	}
	return DRAM_V_FB;
}

int main( int argc, char**  argv )
{
	struct oled_rect _rect;
	struct oled_framebuffer fb;
	int fd = -1;

	fb.size = OLED_FRAMEBUFFER_SIZE;
	fb.pbuf = frame_buffer;

	_rect.x = 0;
	_rect.y = 0;
	_rect.w = 127;
	_rect.h = 31;

	if ( argc < 2 ) {
		usage();
		return 0;
	}

	fd = open(OLED_DEVICE, O_RDWR);
	if(fd < 0){
		fprintf(stderr,"%s open failed \n" , OLED_DEVICE);
		return -1;
	}
	if(0 == strcmp(argv[1], "reset")) {
		ioctl(fd, OLED_RESET,0);
	} else if (0 == strcmp(argv[1], "white")) {
		memset(frame_buffer, 0xFF, sizeof(frame_buffer));
		ioctl(fd, OLED_FILLFB, &fb);
		ioctl(fd, OLED_UPDATERECT, &_rect);
	} else if (0 == strcmp(argv[1], "black")) {
		memset(frame_buffer, 0x00, sizeof(frame_buffer));
		ioctl(fd, OLED_FILLFB, &fb);
		ioctl(fd, OLED_UPDATERECT, &_rect);
	} else if (0 == strcmp(argv[1], "raw")) {
		memcpy(frame_buffer, __Oled_Convert((unsigned char*)init_rawdata), sizeof(frame_buffer));
		ioctl(fd, OLED_FILLFB, &fb);
		ioctl(fd, OLED_UPDATERECT, &_rect);
	} else if (0 == strcmp(argv[1], "image")) {
		int i;
		if(argc != 3) usage();

		for( i = 0; ; i++) {
			if(g_Image[i].pImage == NULL) {
				fprintf(stderr,"%s not find \n" , argv[2]);
				break;
			}
			if( 0 == strcmp(g_Image[i].name, argv[2]) ){
				memcpy(frame_buffer, g_Image[i].pImage, sizeof(frame_buffer));
				ioctl(fd, OLED_FILLFB, &fb);
				ioctl(fd, OLED_UPDATERECT, &_rect);
				break;
			}
		}
	} else {
		usage();
	}
	close(fd);
	return 0;
}


