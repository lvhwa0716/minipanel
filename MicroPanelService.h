
#ifndef __MICROPANELSERVICE_H__
#define __MICROPANELSERVICE_H__

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

namespace android {


	class IMicroPanelService : public IInterface
    {
		public:
		    DECLARE_META_INTERFACE(MicroPanelService);

		    virtual void Rect(int x, int y, int w, int h) = 0;
			virtual void FillRect(int x, int y, int w, int h) = 0;
			virtual void ClearAll() = 0;
			virtual void SetColor(int color) = 0;
			virtual void DrawPixel(int x, int y, int color) = 0;
			virtual int ReadPixel(int x, int y) = 0;
			virtual void HLine(int x1, int x2, int y) = 0; // x1 < x2
			virtual void VLine(int x, int y1, int y2) = 0; // y1 < y2
			virtual void Line(int x1, int y1, int x2, int y2) = 0;
			virtual void DrawString(int x, int y, String16 str) = 0;
			virtual void DrawBitmap(int x, int y, int bmp_w, int bmp_h, int bmp_pitch , int bmp_bpp, unsigned char* bmp) = 0;


			//virtual void Init(void) = 0;
			//virtual void DeInit(void) = 0;
			virtual void Sleep(int level) = 0;
			virtual void Wakeup(void) = 0;
			virtual void Brightness(int b) = 0;
			virtual void UpdateScreen(int x, int y, int w, int h) = 0;
    };

	class BnMicroPanelService: public BnInterface<IMicroPanelService> {
		public:
			virtual status_t onTransact(uint32_t code, const Parcel& data,
				    Parcel* reply, uint32_t flags = 0);
		BnMicroPanelService();
		virtual ~BnMicroPanelService();
	};

	class MicroPanelService :
		public BinderService<MicroPanelService>,
		public BnMicroPanelService
	{
		friend class BinderService<MicroPanelService>;

	public:

		static void instantiate() {
			defaultServiceManager()->addService(
				    String16("tclxa.minipanel"), new MicroPanelService());
		}
		MicroPanelService();
		virtual ~MicroPanelService();

		// IMicroPanelService
		void Rect(int x, int y, int w, int h);
		void FillRect(int x, int y, int w, int h);
		void ClearAll();
		void SetColor(int color);
		void DrawPixel(int x, int y, int color);
		int ReadPixel(int x, int y);
		void HLine(int x1, int x2, int y); // x1 < x2
		void VLine(int x, int y1, int y2); // y1 < y2
		void Line(int x1, int y1, int x2, int y2);
		void DrawString(int x, int y, String16 str);
		void DrawBitmap(int x, int y, int bmp_w, int bmp_h, int bmp_pitch , int bmp_bpp, unsigned char* bmp);


		//void Init(void);
		//void DeInit(void);
		void Sleep(int level);
		void Wakeup(void);
		void Brightness(int b);
		void UpdateScreen(int x, int y, int w, int h);
	}; // class Module

}  

#endif // #ifndef __MICROPANELSERVICE_H__
