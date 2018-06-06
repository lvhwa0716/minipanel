
#define LOG_TAG "MicroPanelService"


#include <string>
#include <vector>
#include <stdint.h>
#include <sys/types.h>

#include <utils/Vector.h>
#include <utils/Errors.h>
//#include <utils/Log.h>
#include <utils/String16.h>
#include <utils/String8.h>
#include <binder/IPCThreadState.h>
#include <binder/Parcel.h>
#include <binder/PermissionCache.h>
#include <private/android_filesystem_config.h>

#include "MicroPanelGui.h"
#include "MicroPanelService.h"

//#define HAS_readByteVector	// new android version

extern "C" unsigned char * OledDriver_intfApp_getFrameBuf(void);

namespace android {  

	void mpGui_DrawString16(int x, int y, String16 str)
	{
		String8 str8(str);
		DBG_LOG("mpGui_DrawString16 : %s\n",str8.string());
		mpGui_DrawString(x, y, (char*)str8.string());
	}
	
	BnMicroPanelService::BnMicroPanelService() {  
		DBG_LOG("MicroPanelService created");  
	}

	BnMicroPanelService::~BnMicroPanelService() {  
		DBG_LOG("MicroPanelService destroyed");  
	}

	status_t BnMicroPanelService::onTransact(uint32_t code,const Parcel& data,Parcel* reply,uint32_t flags) {
		DBG_LOG("BnMicroPanelService onTransact:: code= %d \n", code - IBinder::FIRST_CALL_TRANSACTION);
		switch(code){  
			case MICROPANEL_SLEEP : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					Sleep(0);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_WAKEUP : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					Wakeup();
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_BRIGHTNESS : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int b = data.readInt32();
					Brightness(b);
					return NO_ERROR;
				}
				break;
			case MICROPANEL_UPADTESCREEN : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y = data.readInt32();
					int w = data.readInt32();
					int h = data.readInt32();
					UpdateScreen(x, y, w, h);
					return NO_ERROR;
				}
				break;
			case MICROPANEL_DRAWSTRING : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y = data.readInt32();
					
					String16 str = data.readString16();
					DrawString(x, y, str);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_DRAWBITMAP : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y = data.readInt32();
					int w = data.readInt32();
					int h = data.readInt32();
					int pitch = data.readInt32();
					int bpp = data.readInt32(); // always 1
				#if defined(HAS_readByteVector)
					std::vector<uint8_t> bmData;
					status_t err = data.readByteVector(bmData); 
					if (err != NO_ERROR) {
						return err;
					}
					unsigned char* bmp = (unsigned char*)bmData.data();
				#else
					// Old Android Version android_os_Parcel_writeNative (byte[])
					int32_t length;
					const status_t err = data.readInt32(&length);
					if (err != NO_ERROR) {
						return err;
					}

					unsigned char* bmp = (unsigned char*)data.readInplace(length);
					if (bmp == NULL) {
						
						return -1;
					}
				#endif
					DrawBitmap(x, y, w, h, pitch , bpp, bmp);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_SETCOLOR : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int c = data.readInt32();
					SetColor(c);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_CLEARALL : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					ClearAll();
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_DRAWPIXEL : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y = data.readInt32();
					int c = data.readInt32();
					DrawPixel( x, y, c);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_READPIXEL : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y = data.readInt32();
					int c = ReadPixel(x, y);
					reply->writeInt32(c);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_HLINE : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x1 = data.readInt32();
					int x2 = data.readInt32();
					int y = data.readInt32();
					HLine( x1, x2,  y);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_VLINE : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y1 = data.readInt32();
					int y2 = data.readInt32();
					VLine( x,  y1,  y2);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_LINE : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x1 = data.readInt32();
					int y1 = data.readInt32();
					int x2 = data.readInt32();
					int y2 = data.readInt32();
					Line( x1,  y1,  x2, y2);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_RECT : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y = data.readInt32();
					int w = data.readInt32();
					int h = data.readInt32();
					Rect( x,  y,  w,  h);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_FILLRECT : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int x = data.readInt32();
					int y = data.readInt32();
					int w = data.readInt32();
					int h = data.readInt32();
					FillRect( x,  y,  w,  h);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_FONTSIZE : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int w = data.readInt32();
					int h = data.readInt32();
					FontSize( w,  h);
					return NO_ERROR;  
				}
				break;
			case MICROPANEL_FONTFILE : {
					CHECK_INTERFACE(IMicroPanelService, data, reply);
					int lang = data.readInt32();
					String16 str = data.readString16();
					int result = FontFile(lang, str);
				    reply->writeNoException();
				    reply->writeInt32(result);
					return NO_ERROR;
				}
				break;
			default:
				return BBinder::onTransact(code,data,reply,flags);  
		}  
	}

	// MicroPanelService

	MicroPanelService::MicroPanelService(): BnMicroPanelService()
	{
		mpGui_Init();
	}
	MicroPanelService::~MicroPanelService()
	{
		mpGui_DeInit();
	}

	void MicroPanelService::Rect(int x, int y, int w, int h)
	{
		mpGui_Rect(x, y, w, h);
	}
	void MicroPanelService::FillRect(int x, int y, int w, int h)
	{
		mpGui_FillRect(x, y, w, h);
	}
	void MicroPanelService::ClearAll()
	{
		mpGui_ClearAll();
	}
	void MicroPanelService::SetColor(int color)
	{
		mpGui_SetColor(color);
	}
	void MicroPanelService::DrawPixel(int x, int y, int color)
	{
		mpGui_DrawPixel(x, y, color);
	}
	int MicroPanelService::ReadPixel(int x, int y)
	{
		return mpGui_ReadPixel( x, y);
	}
	void MicroPanelService::HLine(int x1, int x2, int y)
	{
		mpGui_HLine( x1, x2, y);
	}
	void MicroPanelService::VLine(int x, int y1, int y2)
	{
		mpGui_VLine(x, y1, y2);
	}
	void MicroPanelService::Line(int x1, int y1, int x2, int y2)
	{
		mpGui_Line(x1, y1, x2, y2);
	}
	void MicroPanelService::DrawString(int x, int y, String16 str )
	{
		mpGui_DrawString16(x, y, str);
	}
	void MicroPanelService::DrawBitmap(int x, int y, 
			int bmp_w, int bmp_h, int bmp_pitch , 
			int bmp_bpp, unsigned char* bmp)
	{
		mpGui_DrawBitmap(x, y, bmp, bmp_w, bmp_h, bmp_pitch , bmp_bpp);
	}

	void MicroPanelService::Sleep(int level)
	{
		mpGui_Sleep(level);
	}
	void MicroPanelService::Wakeup(void)
	{
		mpGui_Wakeup();
	}
	void MicroPanelService::Brightness(int b)
	{
		mpGui_Brightness(b);
	}
	void MicroPanelService::UpdateScreen(int x, int y, int w, int h)
	{
		mpGui_UpdateScreen( x,  y,  w,  h);
	}
	void MicroPanelService::FontSize(int w, int h)
	{
		mpGui_FontSize( w,  h);
	}
	int MicroPanelService::FontFile(int lang, String16 fontFile)
	{
		String8 str8(fontFile);
		return mpGui_SetFont( lang, (char*)str8.string());
	}
	status_t MicroPanelService::dump(int fd, const Vector<String16>& args)
	{
		String8 result;
		const String16 sDump("android.permission.DUMP");
		IPCThreadState* ipc = IPCThreadState::self();
		const int pid = ipc->getCallingPid();
		const int uid = ipc->getCallingUid();
		if ((uid != AID_SHELL) &&
		        !PermissionCache::checkPermission(sDump, pid, uid)) {
		    result.appendFormat("Permission Denial: "
		            "can't dump MicroPanelService from pid=%d, uid=%d\n", pid, uid);
		} else {
			result.appendFormat("Width : %d \n" , MICROPANEL_WIDTH);
			result.appendFormat("Height : %d \n" , MICROPANEL_HEIGHT);
			result.appendFormat("bit : %d \n" , 1);
			
			int i;
			size_t numArgs = args.size();
			if(numArgs == 0) {
				const unsigned char * pData = OledDriver_intfApp_getFrameBuf();	
				for(i = 1; i <= MICROPANEL_WIDTH * MICROPANEL_HEIGHT * 1 / 8; i++ ) {
					result.appendFormat("0x%02X,",*pData);
					if(i % 16 == 0)	result.appendFormat("\n");
					pData++;
				}
			} else {
				int index = 0;
				if ((index < numArgs) &&
                    (args[index] == String16("image"))) {
		            index++;
		            {
						int y, x;
						int w = MICROPANEL_WIDTH;
						int h = MICROPANEL_HEIGHT;
						const unsigned char * bitmap = OledDriver_intfApp_getFrameBuf();
						const unsigned char dst_mask_[8] = {0x01, 0x02 , 0x04 ,0x08,0x10 ,0x20,0x40,0x80};

						for( y = 0; y < h  ; y++ ) {
							for( x = 0; x < w ; x++ ) {
								int offset = x + (y / 8 ) * 128;
								result.appendFormat("%c", ( bitmap[ offset]  & dst_mask_[y % 8] ? '*' : ' '));
							}
							result.appendFormat("\n");
						}
					}
            	}
			}
		}
		write(fd, result.string(), result.size());
		return NO_ERROR;
	}
}

