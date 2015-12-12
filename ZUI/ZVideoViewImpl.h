#ifndef __ZUIVIDEO_VIEW_H__
#define __ZUIVIDEO_VIEW_H__

extern "C"
{
#include "libavutil/avstring.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
#include "libavfilter/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
}

#include "base/thread.h"
#include "base/RefBase.h"
#include "SDL.h"

namespace ZuiLib
{

class ZVideoPacketQueue
{
public:
	ZVideoPacketQueue();
	~ZVideoPacketQueue();
	int PutPacket(AVPacket *pkt);
	int GetPacket(AVPacket *pkt,bool block);
	void Flush();
	int Size(){return m_size;}
private:
	AVPacketList *m_first_pkt;
	AVPacketList *m_last_pkt;
	int m_nb_packets;
	int m_size;
	SDL_mutex *m_mutex;
	SDL_cond *m_cond;
};

class ZVideoState:public base::RefCountedBase
{
public:
	ZVideoState()
		:pFormatCtx(NULL),pCodecCtx(NULL),pCodecCtxOrig(NULL),videoStream(-1),audioStream(-1),seek_req(0),seek_flags(0),seek_pos(0),quit(false)
	{}

	bool OpenFile(const char* file);
	void CloseFile();
	bool InitMedia();

	void DecodeFrames();//解码线程调用


	AVFormatContext* pFormatCtx;//读取packet使用
	AVCodecContext* pCodecCtx;//对packet解码使用
	AVCodecContext* pCodecCtxOrig;
	int             videoStream;
	int				audioStream;

	ZVideoPacketQueue videoq;
	ZVideoPacketQueue audioq;
	int             seek_req;
	int             seek_flags;
	int64_t         seek_pos;
	bool			quit;
};


class ZVideoViewImpl:public base::MessageHandler
{
public:

	ZVideoViewImpl();
	~ZVideoViewImpl();

	virtual void OnMessage(base::Message *pmsg);

private:
	base::Thread m_videoThread;//工作线程
	base::Thread m_codecThread;//读取packet
	base::RefCountedPtr<ZVideoState> m_videoState;

};

}
#endif