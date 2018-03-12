
#include "MicroPanelGui.h"

#if defined(HTTP_DEBUG_PORT)

#include <utils/threads.h>
#include <utils/List.h>
#include <curl/curl.h>

using namespace android;


class FrameBufferPush : public Thread {
	public:
		FrameBufferPush(): Thread(false) {

		}
		~FrameBufferPush() {
				{
					AutoMutex lock(&mLock);
					requestExit();
					mWaitWorkCV.signal();
				}
				requestExitAndWait();
				clear();
		}
		status_t sendMessage(int msg, int ext1, int ext2, void *obj) {
			
			Mutex::Autolock _l(mLock);
			NotifyMessage *pMsg = new NotifyMessage(msg, ext1, ext2, obj);
			mMsgQueue.push_back(pMsg);
			mWaitWorkCV.signal();
			return NO_ERROR;
		}
		enum NotifyMessageID {
			MSG_START = 0,
			MSG_REFRESH_ALL = 1,
			MSG_END,
		};
	private:
		class NotifyMessage	{
			public:
				NotifyMessage( int msg, int ext1, int ext2, void *obj)
				: mMsg(msg), mExt1(ext1), mExt2(ext2), mObj(obj) {
					
				}
				~NotifyMessage() {
				}
			public:

				int mMsg;
				int mExt1;
				int mExt2;
				void* mObj;
				friend class FrameBufferPush;

		};
		virtual bool threadLoop() {
			while (!exitPending())
			{
				NotifyMessage *p = 0;
				{
					Mutex::Autolock _l(mLock);

					while (mMsgQueue.empty())
					{
						mWaitWorkCV.wait(mLock);
					}

					if (exitPending())
					{
						break;
					}

					List<NotifyMessage *>::iterator i = mMsgQueue.begin();
					p = *i;
					mMsgQueue.erase(i);
				}

				if (p)
				{
					clear(); // clear all pending
					switch(p->mMsg) {
						case MSG_REFRESH_ALL: {
								struct _update_context_ ctx;
								ctx.data = (unsigned char*)p->mObj;
								ctx.size = p->mExt1;
								ctx.pos = 0;
								do_RefreshAll(ctx);
							}
							break;
						default:
							break;
					}
					delete p;
				}

			}

			clear();
			return false;
		}

		void clear() {
			Mutex::Autolock _l(mLock);

			for (List<NotifyMessage *>::iterator i = mMsgQueue.begin(); i != mMsgQueue.end(); ++i) {
				NotifyMessage *p = *i;
				delete p;
			}
			mMsgQueue.clear();
		}
		Mutex       mLock;
		Condition  mWaitWorkCV;
		List <NotifyMessage *> mMsgQueue;

	private:  // refresh all 

		struct _update_context_ {
			unsigned char *data;
			int size;
			int pos;

		} local_ctx;
		static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream) {  
			struct _update_context_ *ctx = (struct _update_context_ *) stream;  
			size_t len = 0;

			if (ctx->pos >= ctx->size) {
				return 0;
			}

			if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
				return 0;
			}

			len = ctx->size - ctx->pos;
			if (len > size*nmemb) {
				len = size * nmemb;
			}

			memcpy(ptr, ctx->data + ctx->pos, len);
			ctx->pos += len;
			DBG_LOG("send len=%d", len);
			return len;
		}
		void do_RefreshAll(struct _update_context_ ctx) {
			CURL *curl = curl_easy_init();
			CURLcode res;
		
			if (curl) {
				char url[255];
				sprintf(url, "http://localhost:%d", HTTP_DEBUG_PORT);
				DBG_LOG("server : %s" , url);

				local_ctx = ctx;

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, FrameBufferPush::read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_PUT, 1L);
				curl_easy_setopt(curl, CURLOPT_URL, url);
				curl_easy_setopt(curl, CURLOPT_READDATA, &local_ctx);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)(local_ctx.size));

				//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

				res = curl_easy_perform(curl);
				if (res != CURLE_OK)
					DBG_ERR( "curl_easy_perform() failed: %s\n",  curl_easy_strerror(res));

				curl_easy_cleanup(curl);
			}
		}
};

static FrameBufferPush* local_FrameBufferPush = 0;

extern "C" void httpDebug_Init() {
	local_FrameBufferPush = new FrameBufferPush();
	local_FrameBufferPush->run("MicroPanelDebugThread", PRIORITY_DEFAULT);
	curl_global_init(CURL_GLOBAL_ALL);
}

extern "C" void httpDebug_DeInit() {
	curl_global_cleanup();
	if( local_FrameBufferPush != 0) {
		delete local_FrameBufferPush;
		local_FrameBufferPush = 0;
	}
}

extern "C" void httpDebug_PushAll(unsigned char * p , int size) {
	if( local_FrameBufferPush == 0 )
		return;

	local_FrameBufferPush->sendMessage(FrameBufferPush::MSG_REFRESH_ALL, size, 0, (void*)p);
}
#endif // HTTP_DEBUG_PORT
