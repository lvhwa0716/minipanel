#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "OledMonoIoctl.h"
#include "OledDriverTest.h"
/***************************************************************************************/

#define OLED_DEVICE "/dev/oled128x32"
extern void OledDriver_intfApp_Init(void);
extern void OledDriver_intfApp_DeInit(void);
extern void OledDriver_intfApp_Sleep(void);
extern void OledDriver_intfApp_WakeUp(void);
extern void OledDriver_intfApp_Brightness(int b);
extern void OledDriver_intfApp_Update(unsigned char *pBuf, int x, int y, int w, int h);
extern int  OledDriver_intfApp_getFd(void);
unsigned char * OledDriver_intfApp_Convert_8bpp(unsigned char *pBuf);
unsigned char * OledDriver_intfApp_Convert_1bpp(unsigned char *pBuf);

extern void bootLoadOLED_FillRect(int x, int y, int w, int h, int c);
extern void bootLoadOLED_Text(int x, int y, char *str , int c);
extern void bootLoadOLED_Update(void);

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
static const unsigned char dst_not_mask_[8] = {0xFE, 0xFD , 0xFB ,0xF7,0xEF ,0xDF,0xBF,0x7F};
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
static unsigned char frameCache[8192];
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
	OledDriver_intfApp_Init();
	fd = OledDriver_intfApp_getFd();
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
	} else if (0 == strcmp(argv[1], "raw1")) {
		memcpy(frame_buffer, OledDriver_intfApp_Convert_1bpp((unsigned char*)init_rawdata), sizeof(frame_buffer));
		ioctl(fd, OLED_FILLFB, &fb);
		ioctl(fd, OLED_UPDATERECT, &_rect);
	}  else if (0 == strcmp(argv[1], "raw8")) {
		memcpy(frame_buffer, OledDriver_intfApp_Convert_8bpp((unsigned char*)init_rawdata_8), sizeof(frame_buffer));
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
	} else if (0 == strcmp(argv[1], "boot")) {

		if(argc != 3) usage();
		bootLoadOLED_FillRect(0, 0, 128, 32, 0);
		bootLoadOLED_Text(4, 4, argv[2] , 1);
		bootLoadOLED_Update();
	} else if (0 == strcmp(argv[1], "dump")) {

		int size = read(fd,frameCache, sizeof(frameCache));
		fprintf ( stderr, "read size = %d \n\n\n" , size);
		FrameBufferDump(frameCache, 128, 32);
		fprintf ( stderr, "\n\n\n");
	} else {
		usage();
	}
	OledDriver_intfApp_DeInit();
	return 0;
}


