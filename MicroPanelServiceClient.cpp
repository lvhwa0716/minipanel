

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <utils/String16.h>

#include <binder/BinderService.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/IBinder.h>
#include <binder/Binder.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include "MicroPanelService.h"

#define DBG_ERR printf

namespace android {



	// IMicroPanelService
	void BpMicroPanelService::Rect(int x, int y, int w, int h) 
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
	void BpMicroPanelService::FillRect(int x, int y, int w, int h) 
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
	void BpMicroPanelService::ClearAll() 
	{
		Parcel data, reply;
		data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());

		if (remote()->transact(MICROPANEL_CLEARALL, data, &reply) != NO_ERROR) {
			DBG_ERR("ClearAll could not contact remote\n");
		}

	}
	void BpMicroPanelService::SetColor(int color) 
	{
		Parcel data, reply;
		data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
		data.writeInt32(color);

		if (remote()->transact(MICROPANEL_SETCOLOR, data, &reply) != NO_ERROR) {
			DBG_ERR("SetColor could not contact remote\n");

		}

	}
	void BpMicroPanelService::DrawPixel(int x, int y, int color) 
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
	int BpMicroPanelService::ReadPixel(int x, int y) 
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
	void BpMicroPanelService::HLine(int x1, int x2, int y) 
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
	void BpMicroPanelService::VLine(int x, int y1, int y2) 
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
	void BpMicroPanelService::Line(int x1, int y1, int x2, int y2) 
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
	void BpMicroPanelService::DrawString(int x, int y, String16 str ) 
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
	void BpMicroPanelService::DrawBitmap(int x, int y, int bmp_w, int bmp_h, int bmp_pitch , int bmp_bpp, unsigned char* bmp) 
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
	void BpMicroPanelService::Sleep(int level)
	{
		Parcel data, reply;
		data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
		data.writeInt32(level);

		if (remote()->transact(MICROPANEL_SLEEP, data, &reply) != NO_ERROR) {
			DBG_ERR("Sleep could not contact remote\n");
		}

	}
	void BpMicroPanelService::Wakeup(void)
	{
		Parcel data, reply;
		data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());

		if (remote()->transact(MICROPANEL_WAKEUP, data, &reply) != NO_ERROR) {
			DBG_ERR("Wakeup could not contact remote\n");

		}

	}
	void BpMicroPanelService::Brightness(int b)
	{
		Parcel data, reply;
		data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
		data.writeInt32(b);


		if (remote()->transact(MICROPANEL_BRIGHTNESS, data, &reply) != NO_ERROR) {
			DBG_ERR("Brightness could not contact remote\n");
		}
	}
	void BpMicroPanelService::UpdateScreen(int x, int y, int w, int h)
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
	void BpMicroPanelService::FontSize(int w, int h)
	{
		Parcel data, reply;
		data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
		data.writeInt32(w);
		data.writeInt32(h);
		if (remote()->transact(MICROPANEL_FONTSIZE, data, &reply) != NO_ERROR) {
			DBG_ERR("FontSize could not contact remote\n");
		}
	}
	int BpMicroPanelService::FontFile(int lang, String16 fontFile)
	{
		Parcel data, reply;
		data.writeInterfaceToken(IMicroPanelService::getInterfaceDescriptor());
		data.writeInt32(lang);
		data.writeString16(fontFile);
		if (remote()->transact(MICROPANEL_FONTFILE, data, &reply) != NO_ERROR) {
			DBG_ERR("FontFile could not contact remote\n");
			return -1;
		}
		return reply.readInt32(); 
	}


	IMPLEMENT_META_INTERFACE(MicroPanelService, "com.i029.minipanel.IMicroPanelService");
}  


