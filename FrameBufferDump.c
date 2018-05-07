#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>



/***************************************************************************************/

// RAW DATA , must full screen

#define OLED_DEVICE "/dev/oled128x32"

static const unsigned char dst_mask_[8] = {0x01, 0x02 , 0x04 ,0x08,0x10 ,0x20,0x40,0x80};

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



int main( int argc, char**argv )
{
	unsigned char frameCache[8192];
	int size;
	int fd = -1;

	argc = argc;
	argv =  argv;

	fd = open(OLED_DEVICE, O_RDONLY);
	if(fd < 0){
		fprintf ( stderr, "%s open failed\n" , OLED_DEVICE);
		return -1;

	}
	size = read(fd,frameCache, sizeof(frameCache));
	close(fd);
	fprintf ( stderr, "read size = %d \n\n\n" , size);
	FrameBufferDump(frameCache, 128, 32);
	fprintf ( stderr, "\n\n\n");
	
	return 0;
}
