#include "ZVideoViewImpl.h"

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avformat.lib")

#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)


namespace ZuiLib
{
	AVPacket flush_pkt;
	ZVideoPacketQueue::ZVideoPacketQueue()
		:m_first_pkt(NULL),m_last_pkt(NULL),m_nb_packets(0),m_size(0)
	{
		m_mutex = SDL_CreateMutex();
		m_cond = SDL_CreateCond();
	}

	ZVideoPacketQueue::~ZVideoPacketQueue()
	{
		Flush();
	}

	int ZVideoPacketQueue::PutPacket(AVPacket *pkt) 
	{
		AVPacketList *pkt1;
		if(pkt != &flush_pkt && av_dup_packet(pkt) < 0) {
			return -1;
		}
		pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
		if (!pkt1)
		return -1;
		pkt1->pkt = *pkt;
		pkt1->next = NULL;
  
		SDL_LockMutex(m_mutex);

		if (!m_last_pkt)
		m_first_pkt = pkt1;
		else
		m_last_pkt->next = pkt1;
		m_last_pkt = pkt1;
		m_nb_packets++;
		m_size += pkt1->pkt.size;
		SDL_CondSignal(m_cond);
		SDL_UnlockMutex(m_mutex);
		return 0;
	}

	int ZVideoPacketQueue::GetPacket(AVPacket *pkt,bool block)
	{
		AVPacketList *pkt1;
		int ret;

		SDL_LockMutex(m_mutex);
  
		for(;;)
		{
			//if(global_video_state->quit)
			//{
			//	ret = -1;
			//	break;
			//}

			pkt1 = m_first_pkt;
			if (pkt1)
			{
				m_first_pkt = pkt1->next;
				if (!m_first_pkt)
					m_last_pkt = NULL;
				m_nb_packets--;
				m_size -= pkt1->pkt.size;
				*pkt = pkt1->pkt;
				av_free(pkt1);
				ret = 1;
				break;
			} else if (!block) {
				ret = 0;
				break;
			} else {
				SDL_CondWait(m_cond, m_mutex);
			}
		}
		SDL_UnlockMutex(m_mutex);
		return ret;
	}

	void ZVideoPacketQueue::Flush()
	{
		AVPacketList *pkt, *pkt1;

		SDL_LockMutex(m_mutex);
		for(pkt = m_first_pkt; pkt != NULL; pkt = pkt1) 
		{
			pkt1 = pkt->next;
			av_free_packet(&pkt->pkt);
			av_freep(&pkt);
		}
		m_last_pkt = NULL;
		m_first_pkt = NULL;
		m_nb_packets = 0;
		m_size = 0;
		SDL_UnlockMutex(m_mutex);
	}

	static void InitLib()
	{
		static bool inited=false;
		if (!inited)
		{
			avcodec_register_all();
			avfilter_register_all();
			av_register_all();

			av_init_packet(&flush_pkt);
			flush_pkt.data = (uint8_t*)"FLUSH";
			inited=true;
		}
	}


	////////////////////////////////////////////////////////////////////////////////////


	bool ZVideoState::OpenFile(const char* file)
	{
		CloseFile();
		if(avformat_open_input(&pFormatCtx, file, NULL,NULL) ==0)
		{
			if(avformat_find_stream_info(pFormatCtx, NULL) >=0 && InitMedia())
			{
				//开启解码线程
				return true;
			}
			else
			{
				CloseFile();
			}
		}
		return false;
	}


	void ZVideoState::CloseFile()
	{
		avcodec_close(pCodecCtx);
		avcodec_close(pCodecCtxOrig);
		avformat_close_input(&pFormatCtx);
		pCodecCtx=NULL;
		pCodecCtxOrig=NULL;
		pFormatCtx=NULL;
	}


	bool ZVideoState::InitMedia()
	{
		int videoStream=-1;
		AVCodecContext* pCodecCtxOrig = NULL;
		AVCodec* pCodec = NULL;
		for(int i=0; i<pFormatCtx->nb_streams; i++)
		{
			if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) 
			{
				videoStream=i;
				break;
			}
		}

		if(videoStream==-1)
			return false; // Didn't find a video stream

		// Get a pointer to the codec context for the video stream
		pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;
		// Find the decoder for the video stream
		pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
		if(pCodec==NULL) {
			fprintf(stderr, "Unsupported codec!\n");
			return -1; // Codec not found
		}
		// Copy context
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
			fprintf(stderr, "Couldn't copy codec context");
			return -1; // Error copying codec context
		}

		// Open codec
		if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
			return false; // Could not open codec
		return true;
	}

	//读取帧数据
	void ZVideoState::DecodeFrames()
	{
		AVPacket pkt1;
		AVPacket*packet = &pkt1;
		for(;;) 
		{
			if(quit)
				break;
			// seek stuff goes here
			if(seek_req) 
			{
				int stream_index= -1;
				int64_t seek_target = seek_pos;

				if(videoStream >= 0) 
					stream_index = videoStream;
				else if(audioStream >= 0) 
					stream_index = audioStream;

				AVRational avr={1, AV_TIME_BASE};
				if(stream_index>=0){
					seek_target= av_rescale_q(seek_target, avr,
							pFormatCtx->streams[stream_index]->time_base);
				}
				if(av_seek_frame(pFormatCtx, stream_index, 
						seek_target, seek_flags) < 0) 
				{
					fprintf(stderr, "%s: error while seeking\n",pFormatCtx->filename);
				}
				else 
				{
					if(audioStream >= 0) 
					{
						audioq.Flush();
						audioq.PutPacket(&flush_pkt);
					}
					if(videoStream >= 0)
					{
						videoq.Flush();
						videoq.PutPacket(&flush_pkt);
					}
				}
				seek_req = 0;
			}

			if(audioq.Size() > MAX_AUDIOQ_SIZE ||
				videoq.Size() > MAX_VIDEOQ_SIZE) {
				SDL_Delay(10);
				continue;
			}
			if(av_read_frame(pFormatCtx, packet) < 0) {
				if(pFormatCtx->pb->error == 0) {
					SDL_Delay(100); /* no error; wait for user input */
					continue;
				} else {
					break;
				}
			}

			// Is this a packet from the video stream?
			if(packet->stream_index == videoStream) {
				videoq.PutPacket(packet);
			} else if(packet->stream_index == audioStream) {
				audioq.PutPacket(packet);
			} else {
				av_free_packet(packet);
			}
		}
	}












	ZVideoViewImpl::ZVideoViewImpl()
	{
		InitLib();
	}

	ZVideoViewImpl::~ZVideoViewImpl()
	{

	}

	void ZVideoViewImpl::OnMessage(base::Message *pmsg)
	{

	}












}