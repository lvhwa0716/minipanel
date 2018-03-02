
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

#include "MicroPanelGui.h"
#include "MicroPanelService.h"

//#define HAS_readByteVector	// new android version

namespace android {  

	enum{  
		MICROPANEL_SLEEP		= IBinder::FIRST_CALL_TRANSACTION + 0,
		MICROPANEL_WAKEUP		= IBinder::FIRST_CALL_TRANSACTION + 1, 
		MICROPANEL_BRIGHTNESS	= IBinder::FIRST_CALL_TRANSACTION + 2,
		MICROPANEL_UPADTESCREEN = IBinder::FIRST_CALL_TRANSACTION + 3,
		MICROPANEL_DRAWSTRING	= IBinder::FIRST_CALL_TRANSACTION + 4,
		MICROPANEL_DRAWBITMAP	= IBinder::FIRST_CALL_TRANSACTION + 5,
		MICROPANEL_SETCOLOR		= IBinder::FIRST_CALL_TRANSACTION + 6,
	// following can do in app
		MICROPANEL_CLEARALL		= IBinder::FIRST_CALL_TRANSACTION + 7,
		MICROPANEL_DRAWPIXEL	= IBinder::FIRST_CALL_TRANSACTION + 8,
		MICROPANEL_READPIXEL	= IBinder::FIRST_CALL_TRANSACTION + 9,
		MICROPANEL_HLINE		= IBinder::FIRST_CALL_TRANSACTION + 10,
		MICROPANEL_VLINE		= IBinder::FIRST_CALL_TRANSACTION + 11,
		MICROPANEL_LINE			= IBinder::FIRST_CALL_TRANSACTION + 12,
		MICROPANEL_RECT			= IBinder::FIRST_CALL_TRANSACTION + 13,
		MICROPANEL_FILLRECT		= IBinder::FIRST_CALL_TRANSACTION + 14,
		MICROPANEL_FONTSIZE		= IBinder::FIRST_CALL_TRANSACTION + 15,

	};
	
	void mpGui_DrawString16(int x, int y, String16 str)
	{
		String8 str8(str);
		DBG_LOG("mpGui_DrawString16 : %s\n",str8.string());
		mpGui_DrawString(x, y, (char*)str8.string());
	}
	class BpMicroPanelService: public BpInterface<IMicroPanelService>
	{
		public:
			BpMicroPanelService(const sp<IBinder>& impl)
				: BpInterface<IMicroPanelService>(impl)
			{
			}

			// IMicroPanelService
			void Rect(int x, int y, int w, int h) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y);
				data.writeInt32(w);
				data.writeInt32(h);
				if (remote()->transact(MICROPANEL_RECT, data, &reply) != NO_ERROR) {
					DBG_ERR("Rect could not contact remote\n");
				}

			}
			void FillRect(int x, int y, int w, int h) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y);
				data.writeInt32(w);
				data.writeInt32(h);
				if (remote()->transact(MICROPANEL_FILLRECT, data, &reply) != NO_ERROR) {
					DBG_ERR("FillRect could not contact remote\n");
				}

			}
			void ClearAll() 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());

				if (remote()->transact(MICROPANEL_CLEARALL, data, &reply) != NO_ERROR) {
					DBG_ERR("ClearAll could not contact remote\n");
				}

			}
			void SetColor(int color) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(color);

				if (remote()->transact(MICROPANEL_SETCOLOR, data, &reply) != NO_ERROR) {
					DBG_ERR("SetColor could not contact remote\n");

				}

			}
			void DrawPixel(int x, int y, int color) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y);
				data.writeInt32(color);
				if (remote()->transact(MICROPANEL_DRAWPIXEL, data, &reply) != NO_ERROR) {
					DBG_ERR("DrawPixel could not contact remote\n");
	
				}

			}
			int ReadPixel(int x, int y) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y);

				if (remote()->transact(MICROPANEL_READPIXEL, data, &reply) != NO_ERROR) {
					DBG_ERR("ReadPixel could not contact remote\n");
					return 0;
				}

				return reply.readInt32();
			}
			void HLine(int x1, int x2, int y) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x1);
				data.writeInt32(x2);
				data.writeInt32(y);

				if (remote()->transact(MICROPANEL_HLINE, data, &reply) != NO_ERROR) {
					ALOGD("HLine could not contact remote\n");
				}

			}
			void VLine(int x, int y1, int y2) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y1);
				data.writeInt32(y2);
				if (remote()->transact(MICROPANEL_VLINE, data, &reply) != NO_ERROR) {
					DBG_ERR("VLine could not contact remote\n");

				}

			}
			void Line(int x1, int y1, int x2, int y2) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x1);
				data.writeInt32(y1);
				data.writeInt32(x2);
				data.writeInt32(y2);
				if (remote()->transact(MICROPANEL_LINE, data, &reply) != NO_ERROR) {
					ALOGD("Line could not contact remote\n");
				}

			}
			void DrawString(int x, int y, String16 str ) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y);
				data.writeString16(str);
				if (remote()->transact(MICROPANEL_DRAWSTRING, data, &reply) != NO_ERROR) {
					DBG_ERR("DrawString could not contact remote\n");

				}

			}
			void DrawBitmap(int x, int y, int bmp_w, int bmp_h, int bmp_pitch , int bmp_bpp, unsigned char* bmp) 
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y);
				data.writeInt32(bmp_w);
				data.writeInt32(bmp_h);
				data.writeInt32(bmp_pitch);
				data.writeInt32(bmp_bpp);
				int32_t length = bmp_pitch * bmp_h;
			#if defined(HAS_readByteVector)
					std::vector<uint8_t> bmData( length);
					memcpy(bmData.data(), bmp, length);
					status_t err = data.writeByteVector(bmData); 
					if (err != NO_ERROR) {
						return ;
					}
			#else
					
					data.writeInt32(length);

					unsigned char* bmData = (unsigned char*)data.writeInplace(length);
					memcpy(bmData, bmp, length);

			#endif
				if (remote()->transact(MICROPANEL_DRAWBITMAP, data, &reply) != NO_ERROR) {
					DBG_ERR("DrawBitmap could not contact remote\n");
				}

			}


			//void Init(void);
			//void DeInit(void);
			void Sleep(int level)
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(level);

				if (remote()->transact(MICROPANEL_SLEEP, data, &reply) != NO_ERROR) {
					DBG_ERR("Sleep could not contact remote\n");
				}

			}
			void Wakeup(void)
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());

				if (remote()->transact(MICROPANEL_WAKEUP, data, &reply) != NO_ERROR) {
					DBG_ERR("Wakeup could not contact remote\n");

				}

			}
			void Brightness(int b)
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(b);


				if (remote()->transact(MICROPANEL_BRIGHTNESS, data, &reply) != NO_ERROR) {
					DBG_ERR("Brightness could not contact remote\n");
				}
			}
			void UpdateScreen(int x, int y, int w, int h)
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(x);
				data.writeInt32(y);
				data.writeInt32(w);
				data.writeInt32(h);
				if (remote()->transact(MICROPANEL_UPADTESCREEN, data, &reply) != NO_ERROR) {
					DBG_ERR("UpdateScreen could not contact remote\n");
				}
			}
			void FontSize(int w, int h)
			{
				Parcel data, reply;
				data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
				data.writeInt32(w);
				data.writeInt32(h);
				if (remote()->transact(MICROPANEL_FONTSIZE, data, &reply) != NO_ERROR) {
					DBG_ERR("FontSize could not contact remote\n");
				}
			}

	};
	
	IMPLEMENT_META_INTERFACE(MicroPanelService, "com.i029.minipanel.IMicroPanelService");

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

}

