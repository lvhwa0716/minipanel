#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>



/***************************************************************************************/

// RAW DATA , must full screen
#define OLED_BUFFER_GPIO "/sys/class/misc/oled128x32/oled_raw"
#define OLED_BUFFER_SPI "/sys/class/spi_master/spi0/spi0.1/oled_raw"

#define OLED_DEVICE "/dev/oled128x32"

void FrameBufferDump(unsigned char *bitmap, int w, int h, int bpl, int bpp)
{
	int  i, j;

	unsigned char *image = bitmap;

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



int main( int argc, char**argv )
{
	unsigned char frameCache[8192];
	int size;
	int fd = -1;

	argc = argc;
	argv =  argv;

	fd = open(OLED_DEVICE, O_RDONLY);
	if(fd < 0){
		fprintf ( stderr, "%s open failed,  try gpio attr\n" , OLED_DEVICE);
		#if (MICROPANEL_BPP == 1)
			fd = open(OLED_BUFFER_GPIO, O_RDONLY);
			if(fd < 0){
				fprintf ( stderr, "%s open failed,  try spi attr\n" , OLED_BUFFER_GPIO);
				fd = open(OLED_BUFFER_SPI, O_RDONLY);
				if(fd < 0){
					fprintf ( stderr, "%s open failed , exist\n" , OLED_BUFFER_SPI);
					return -1;
				}
			}
		#else
			return -1;
		#endif
	}
	size = read(fd,frameCache, sizeof(frameCache));
	fprintf ( stderr, "read size = %d \n\n\n" , size);
	FrameBufferDump(frameCache, 128, 32, 128 / 8 * MICROPANEL_BPP, MICROPANEL_BPP);
	fprintf ( stderr, "\n\n\n");
	close(fd);
	return 0;
}
