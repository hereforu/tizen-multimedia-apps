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
:m_total_audio_packet_after_demuxing(0), m_total_video_packet_after_demuxing(0), m_bcreated(false)
{

}
TranscodingEngine::~TranscodingEngine()
{

}

void TranscodingEngine::Create(const char* srcfilename, CodecInfo& venc, CodecInfo& aenc)
{
	print_errorcode_for_debug();
	m_dstfilename = generatedstfilename(srcfilename);
	m_vencinfo = venc;
	m_aencinfo = aenc;

	try
	{
		createdemuxer(srcfilename);
		createcodec(venc, aenc);
		m_bcreated = true;
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "TranscodingEngine", e.what());
		throw e;
	}
}
void TranscodingEngine::Destroy()
{
	try
	{
		m_demuxer.Destroy();
		m_vdecoder.Destroy();
		m_vencoder.Destroy();
	//	m_adecoder.Destroy();
	//	m_aencoder.Destroy();
		m_queue[AFTER_DEMUXING_QUEUE][VIDEO_TRACK].ClearAll();
		m_queue[AFTER_DEMUXING_QUEUE][AUDIO_TRACK].ClearAll();
		m_queue[AFTER_DECODING_QUEUE][VIDEO_TRACK].ClearAll();
		m_queue[AFTER_DECODING_QUEUE][AUDIO_TRACK].ClearAll();
		m_queue[AFTER_ENCODING_QUEUE][VIDEO_TRACK].ClearAll();
		m_queue[AFTER_ENCODING_QUEUE][AUDIO_TRACK].ClearAll();
		m_bcreated = false;
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "TranscodingEngine", e.what());
		throw e;
	}
}

bool TranscodingEngine::IsCreated()
{
	return m_bcreated;
}

void TranscodingEngine::Start()
{
	m_demuxer.Start();
	capture_current_packet_state_after_demuxing();
	m_vdecoder.Start();
//	m_adecoder.Start();
	m_vencoder.Start();
//	m_aencoder.Start();

	while(m_vencoder.IsEoS() == false) //end condition
	{
		sleep(1);
	}
}
void TranscodingEngine::Stop()
{

}
double TranscodingEngine::GetProgress()
{
	double progress = 0.0;
	if(m_total_video_packet_after_demuxing != 0.0)
		progress = (double)m_queue[AFTER_ENCODING_QUEUE][VIDEO_TRACK].Size()/(double)m_total_video_packet_after_demuxing;
	if(m_vencoder.IsEoS())
		progress = 1.0;
	return progress;
}
void TranscodingEngine::print_errorcode_for_debug()
{
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_OUT_OF_MEMORY:%d", MEDIAMUXER_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_INVALID_PARAMETER:%d", MEDIAMUXER_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_INVALID_OPERATION:%d", MEDIAMUXER_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_NOT_SUPPORTED:%d", MEDIAMUXER_ERROR_NOT_SUPPORTED);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_PERMISSION_DENIED:%d", MEDIAMUXER_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_INVALID_STATE:%d", MEDIAMUXER_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_INVALID_PATH:%d", MEDIAMUXER_ERROR_INVALID_PATH);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIAMUXER_ERROR_RESOURCE_LIMIT:%d", MEDIAMUXER_ERROR_RESOURCE_LIMIT);

	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_OUT_OF_MEMORY:%d", MEDIADEMUXER_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_INVALID_PARAMETER:%d", MEDIADEMUXER_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_INVALID_OPERATION:%d", MEDIADEMUXER_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_NOT_SUPPORTED:%d", MEDIADEMUXER_ERROR_NOT_SUPPORTED);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_PERMISSION_DENIED:%d", MEDIADEMUXER_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_INVALID_STATE:%d", MEDIADEMUXER_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_INVALID_PATH:%d", MEDIADEMUXER_ERROR_INVALID_PATH);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_RESOURCE_LIMIT:%d", MEDIADEMUXER_ERROR_RESOURCE_LIMIT);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_SEEK_FAILED:%d", MEDIADEMUXER_ERROR_SEEK_FAILED);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIADEMUXER_ERROR_DRM_NOT_PERMITTED:%d", MEDIADEMUXER_ERROR_DRM_NOT_PERMITTED);

	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_OUT_OF_MEMORY:%d", MEDIACODEC_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_INVALID_PARAMETER:%d", MEDIACODEC_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_INVALID_OPERATION:%d", MEDIACODEC_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_NOT_SUPPORTED_ON_DEVICE:%d", MEDIACODEC_ERROR_NOT_SUPPORTED_ON_DEVICE);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_PERMISSION_DENIED:%d", MEDIACODEC_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_INVALID_STATE:%d", MEDIACODEC_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_INVALID_INBUFFER:%d", MEDIACODEC_ERROR_INVALID_INBUFFER);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_INVALID_OUTBUFFER:%d", MEDIACODEC_ERROR_INVALID_OUTBUFFER);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_INTERNAL:%d", MEDIACODEC_ERROR_INTERNAL);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_NOT_INITIALIZED:%d", MEDIACODEC_ERROR_NOT_INITIALIZED);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_INVALID_STREAM:%d", MEDIACODEC_ERROR_INVALID_STREAM);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_CODEC_NOT_FOUND:%d", MEDIACODEC_ERROR_CODEC_NOT_FOUND);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_DECODE:%d", MEDIACODEC_ERROR_DECODE);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_NO_FREE_SPACE:%d", MEDIACODEC_ERROR_NO_FREE_SPACE);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_STREAM_NOT_FOUND:%d", MEDIACODEC_ERROR_STREAM_NOT_FOUND);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_NOT_SUPPORTED_FORMAT:%d", MEDIACODEC_ERROR_NOT_SUPPORTED_FORMAT);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_BUFFER_NOT_AVAILABLE:%d", MEDIACODEC_ERROR_BUFFER_NOT_AVAILABLE);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_OVERFLOW_INBUFFER:%d", MEDIACODEC_ERROR_OVERFLOW_INBUFFER);
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "MEDIACODEC_ERROR_RESOURCE_OVERLOADED:%d", MEDIACODEC_ERROR_RESOURCE_OVERLOADED);
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

		if(venc.venc.codecid == 0)//ORIGINAL_FEATURE
			venc.venc.codecid = vdec.vdec.codecid;
		if(aenc.aenc.codecid == 0)//ORIGINAL_FEATURE
			aenc.aenc.codecid = adec.adec.codecid;
		aenc.aenc.bit = adec.adec.bit;
		aenc.aenc.channel = adec.adec.channel;
		aenc.aenc.samplerate = adec.adec.samplerate;
		aenc.aenc.bitrate = aenc.aenc.bit*aenc.aenc.channel*aenc.aenc.samplerate;
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "video decode: codecid:%d, width:%d, height:%d", vdec.vdec.codecid, vdec.vdec.width, vdec.vdec.height);
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "video encode: codecid:%d, width:%d, height:%d, fps:%d, bitrate:%d"
				, venc.venc.codecid, venc.venc.width, venc.venc.height, venc.venc.fps, venc.venc.target_bits);
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "audio decode: codecid:%d, bit:%d, channel:%d, samplerate:%d"
						, adec.adec.codecid, adec.adec.bit, adec.adec.channel, adec.adec.samplerate);
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "audio encode: codecid:%d, bit:%d, channel:%d, samplerate:%d, bitrate:%d"
								, aenc.aenc.codecid, aenc.aenc.bit, aenc.aenc.channel, aenc.aenc.samplerate, aenc.aenc.bitrate);

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

void TranscodingEngine::capture_current_packet_state_after_demuxing()
{
	m_total_video_packet_after_demuxing = m_queue[AFTER_DEMUXING_QUEUE][VIDEO_TRACK].Size();
	m_total_audio_packet_after_demuxing = m_queue[AFTER_DEMUXING_QUEUE][AUDIO_TRACK].Size();
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


