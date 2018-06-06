#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <utils/String16.h>
#include <utils/String8.h>
#include "MicroPanelService.h"

/***************************************************************************************/
using namespace android;
int main( int argc, char**  argv )
{
	sp<android::IServiceManager> sm = defaultServiceManager();  
	sp<IBinder> binder = sm->getService(String16("i029.minipanel"));
	if(binder == 0) {
		printf("service not start.");
		return -1;
	}
	
	BpMicroPanelService service(binder);

	service.SetColor(0);
	service.FillRect(0,0,127,31);
	service.SetColor(1);
	service.DrawString(4,0,String16("Service Test"));
	service.FontFile(0,String16("/system/fonts/Roboto-BoldItalic.ttf"));	
	service.DrawString(4,17,String16("Service Test"));
	service.UpdateScreen(0,0,127,31);
	return 0;
}


