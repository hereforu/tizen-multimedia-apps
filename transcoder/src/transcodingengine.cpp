/*
 * transcodingengine.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#include "base.h"
#include "transcodingengine.h"
#include "demuxer.h"

#include "muxer.h"
#include <stdexcept>

TranscodingEngine::TranscodingEngine()
:m_bcreated(false)
{

}
TranscodingEngine::~TranscodingEngine()
{

}

void TranscodingEngine::Create(const char* srcfilename, CodecInfo& venc, CodecInfo& aenc)
{
	m_dstfilename = generatedstfilename(srcfilename);
	m_vencinfo = venc;
	m_aencinfo = aenc;

	try
	{
		createdemuxer(srcfilename);
		createcodec(venc, aenc);
	}
	catch(const std::runtime_error& e)
	{
		throw e;
	}
	m_bcreated = true;

}
void TranscodingEngine::Destroy()
{
	m_demuxer.Destroy();
	m_vdecoder.Destroy();
	m_vencoder.Destroy();
	m_adecoder.Destroy();
	m_aencoder.Destroy();
	m_queue[AFTER_DEMUXING_QUEUE][VIDEO_TRACK].ClearAll();
	m_queue[AFTER_DEMUXING_QUEUE][AUDIO_TRACK].ClearAll();
	m_queue[AFTER_DECODING_QUEUE][VIDEO_TRACK].ClearAll();
	m_queue[AFTER_DECODING_QUEUE][AUDIO_TRACK].ClearAll();
	m_queue[AFTER_ENCODING_QUEUE][VIDEO_TRACK].ClearAll();
	m_queue[AFTER_ENCODING_QUEUE][AUDIO_TRACK].ClearAll();
	m_bcreated = false;
}
bool TranscodingEngine::IsCreated()
{
	return m_bcreated;
}
void TranscodingEngine::Start()
{
	m_demuxer.Start();
	m_vdecoder.Start();
//	m_adecoder.Start();
	m_vencoder.Start();
//	m_aencoder.Start();

}
void TranscodingEngine::Stop()
{

}
int TranscodingEngine::GetProgress()
{
	return 0;
}
void TranscodingEngine::createdemuxer(const char* srcfilename)
{
	try
	{
		m_demuxer.Create(srcfilename);
		m_demuxer.SetVideoQueue(&m_queue[AFTER_DEMUXING_QUEUE][VIDEO_TRACK]);
		m_demuxer.SetAudioQueue(&m_queue[AFTER_DEMUXING_QUEUE][AUDIO_TRACK]);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "TranscodingEngine", e.what());
	//	throw e;
	}
}
void TranscodingEngine::createcodec(CodecInfo& venc, CodecInfo& aenc)
{
	try
	{
		CodecInfo vdec, adec;
		m_demuxer.GetVideoDecInfo(vdec);
		m_demuxer.GetAudioDecInfo(adec);
		m_vdecoder.Create(vdec, &m_queue[AFTER_DEMUXING_QUEUE][VIDEO_TRACK], &m_queue[AFTER_DECODING_QUEUE][VIDEO_TRACK]);
	//	m_adecoder.Create(adec, &m_queue[AFTER_DEMUXING_QUEUE][AUDIO_TRACK], &m_queue[AFTER_DECODING_QUEUE][AUDIO_TRACK]);
		m_vencoder.Create(venc, &m_queue[AFTER_DECODING_QUEUE][VIDEO_TRACK], &m_queue[AFTER_ENCODING_QUEUE][VIDEO_TRACK]);
	//	m_aencoder.Create(aenc, &m_queue[AFTER_DECODING_QUEUE][AUDIO_TRACK], &m_queue[AFTER_ENCODING_QUEUE][AUDIO_TRACK]);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "TranscodingEngine", e.what());
	//	throw e;
	}
}
const char* TranscodingEngine::generatedstfilename(const char* srcfilename)
{
	return "/home/owner/result.mp4";
}


#if 0

int num_packets = 0;
void ReadSample(void* data, int track_index, media_packet_h* inbuf, bool* eos)
{
	SharedQueue* queue = (SharedQueue*)data;
	*inbuf = queue->Get();
	*eos = false;
	dlog_print(DLOG_DEBUG, "CodecBase", "input %dth packet into muxer", num_packets++);
	if(queue->JustSee()==0)
	{
		dlog_print(DLOG_DEBUG, "CodecBase", "ReadSample is EOS");
		*eos = true;
		media_packet_h eospacket = queue->Get();
	}
}

void start_transcoding(const char* srcfilename, CodecInfo& venc, CodecInfo& aenc)
{
	int ret;
	Demuxer demuxer;
	SharedQueue videoqueue;
	SharedQueue audioqueue;

	SharedQueue video_enc_queue;
	SharedQueue audio_enc_queue;

	SharedQueue video_trans_queue;
	SharedQueue audio_trans_queue;

	CodecInfo vdec, adec;

	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_OUT_OF_MEMORY:%d", MEDIADEMUXER_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_INVALID_PARAMETER:%d", MEDIADEMUXER_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_INVALID_OPERATION:%d", MEDIADEMUXER_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_NOT_SUPPORTED:%d", MEDIADEMUXER_ERROR_NOT_SUPPORTED);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_PERMISSION_DENIED:%d", MEDIADEMUXER_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_INVALID_STATE:%d", MEDIADEMUXER_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_INVALID_PATH:%d", MEDIADEMUXER_ERROR_INVALID_PATH);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_RESOURCE_LIMIT:%d", MEDIADEMUXER_ERROR_RESOURCE_LIMIT);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_SEEK_FAILED:%d", MEDIADEMUXER_ERROR_SEEK_FAILED);
	dlog_print(DLOG_DEBUG, "Demuxer", "MEDIADEMUXER_ERROR_DRM_NOT_PERMITTED:%d", MEDIADEMUXER_ERROR_DRM_NOT_PERMITTED);
	try
	{
		demuxer.Create(srcfilename);
		int numtracks = demuxer.GetNumTracks();
		dlog_print(DLOG_DEBUG, "Demuxer", "num of tracks:%d", numtracks);

		media_format_h vformat = demuxer.GetMediaFormat(0);
		media_format_mimetype_e vmimetype;
		int width, height, avg_bps, max_bps;
		ret = media_format_get_video_info(vformat, &vmimetype, &width, &height, &avg_bps, &max_bps);
		dlog_print(DLOG_DEBUG, "Demuxer", "get video track info (%d)", ret);
		vdec.vdec.codecid = (mediacodec_codec_type_e) (vmimetype & 0x0000FFFF);
		vdec.vdec.width = width;
		vdec.vdec.height = height;

		media_format_h aformat = demuxer.GetMediaFormat(1);
		media_format_mimetype_e amimetype;
		int channel, samplerate, bit;
		ret = media_format_get_audio_info(aformat, &amimetype, &channel, &samplerate, &bit, &avg_bps);
		adec.adec.codecid = (mediacodec_codec_type_e)  (amimetype & 0x0000FFFF);
		adec.adec.samplerate = samplerate;
		adec.adec.channel = channel;
		adec.adec.bit = bit;
		dlog_print(DLOG_DEBUG, "Demuxer", "get audio track info (%d)", ret);

		demuxer.SetQueue(0, &videoqueue);
		demuxer.SetQueue(1, &audioqueue);
		demuxer.Start();
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_DEBUG, "Demuxer", e.what());
		return;
	}
	demuxer.Destroy();
	dlog_print(DLOG_DEBUG, "Demuxer", "size of video queue after demuxing:%d", videoqueue.Size());
	dlog_print(DLOG_DEBUG, "Demuxer", "size of audio queue after demuxing:%d", audioqueue.Size());


	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_OUT_OF_MEMORY:%d", MEDIACODEC_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_INVALID_PARAMETER:%d", MEDIACODEC_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_INVALID_OPERATION:%d", MEDIACODEC_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_NOT_SUPPORTED_ON_DEVICE:%d", MEDIACODEC_ERROR_NOT_SUPPORTED_ON_DEVICE);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_PERMISSION_DENIED:%d", MEDIACODEC_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_INVALID_STATE:%d", MEDIACODEC_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_INVALID_INBUFFER:%d", MEDIACODEC_ERROR_INVALID_INBUFFER);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_INVALID_OUTBUFFER:%d", MEDIACODEC_ERROR_INVALID_OUTBUFFER);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_INTERNAL:%d", MEDIACODEC_ERROR_INTERNAL);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_NOT_INITIALIZED:%d", MEDIACODEC_ERROR_NOT_INITIALIZED);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_INVALID_STREAM:%d", MEDIACODEC_ERROR_INVALID_STREAM);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_CODEC_NOT_FOUND:%d", MEDIACODEC_ERROR_CODEC_NOT_FOUND);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_DECODE:%d", MEDIACODEC_ERROR_DECODE);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_NO_FREE_SPACE:%d", MEDIACODEC_ERROR_NO_FREE_SPACE);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_STREAM_NOT_FOUND:%d", MEDIACODEC_ERROR_STREAM_NOT_FOUND);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_NOT_SUPPORTED_FORMAT:%d", MEDIACODEC_ERROR_NOT_SUPPORTED_FORMAT);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_BUFFER_NOT_AVAILABLE:%d", MEDIACODEC_ERROR_BUFFER_NOT_AVAILABLE);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_OVERFLOW_INBUFFER:%d", MEDIACODEC_ERROR_OVERFLOW_INBUFFER);
	dlog_print(DLOG_DEBUG, "CodecBase", "MEDIACODEC_ERROR_RESOURCE_OVERLOADED:%d", MEDIACODEC_ERROR_RESOURCE_OVERLOADED);


	//transcoding
	VideoDecoder vdecoder;
	VideoEncoder vencoder;
	AudioDecoder adecoder;
	AudioEncoder aencoder;

	try
	{
		vdecoder.Create(vdec, &videoqueue, &video_enc_queue);
		vdecoder.Start();
	//	adecoder.Create(adec, &audioqueue, &audio_enc_queue);
	//	adecoder.Start();

		vencoder.Create(venc, &video_enc_queue, &video_trans_queue);
		vencoder.Start();
	//	aencoder.Create(aenc, &audio_enc_queue, &audio_trans_queue);
	//	aencoder.Start();


		while(vdecoder.IsEoS() == false || vencoder.IsEoS() == false)
		{
			dlog_print(DLOG_DEBUG, "CodecBase", "vdecoder.IsEoS():%d, vencoder.IsEoS():%d", vdecoder.IsEoS(), vencoder.IsEoS());
			sleep(1);
		}
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_DEBUG, "CodecBase", e.what());
		return;
	}
	vencoder.Destroy();
	vdecoder.Destroy();
	dlog_print(DLOG_DEBUG, "CodecBase", "size of input video queue after decoding:%d", videoqueue.Size());
	dlog_print(DLOG_DEBUG, "CodecBase", "size of output video queue after decoding:%d", video_enc_queue.Size());
	dlog_print(DLOG_DEBUG, "CodecBase", "size of output video queue after encoding:%d", video_trans_queue.Size());

	CreateMuxer("/home/owner/wow.mp4", MEDIAMUXER_CONTAINER_FORMAT_MP4);
	AddTrack(vencoder.GetMediaFormat());
	StartMuxer(ReadSample, (void*)&video_trans_queue);
	DestroyMuxer();
}

#endif


