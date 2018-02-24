

#define LOG_TAG "MicroPanelService"
//#define LOG_NDEBUG 0

#include <sys/types.h>
#include <unistd.h>

//#include <private/android_filesystem_config.h>

#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <cutils/properties.h>
#include <utils/Log.h>


// from LOCAL_C_INCLUDES
#include "MicroPanelService.h"



using namespace android;

int main(void)
{
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();
	ALOGI("ServiceManager: %p", sm.get());

	MicroPanelService::instantiate();

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
	return 0;
}
