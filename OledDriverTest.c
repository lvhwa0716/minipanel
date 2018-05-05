#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "OledMonoIoctl.h"

/***************************************************************************************/

#define OLED_DEVICE "/dev/oled128x32"

int main( int argc, char**  argv )
{

	int fd = -1;
	fd = open(OLED_DEVICE, O_RDWR);
	if(fd < 0){
		fprintf("%s open failed \n" , OLED_DEVICE);
		return -1;
	}
	ioctl(fd, OLED_RESET,0);
/*
	if ( argc == 1 )
	{
		fprintf ( stderr, "usage: %s text\n", argv[0] );
		return ;
	}
*/

	return 0;
}

