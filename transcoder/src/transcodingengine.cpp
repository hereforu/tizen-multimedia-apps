/*
 * transcodingengine.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "transcodingengine.h"
#include "demuxer.h"

#include "muxer.h"
#include <stdexcept>

//#define IMAGE_RESIZER_ON


TranscodingEngine::TranscodingEngine()
:m_progress_count(0), m_estimated_packets(0),m_muxer_video_track_index(-1), m_muxer_audio_track_index(-1), m_bcreated(false)
{

}
TranscodingEngine::~TranscodingEngine()
{

}

void TranscodingEngine::Create(const char* srcfilename, unsigned int duration, CodecInfo& venc, CodecInfo& aenc)
{
	print_errorcode_for_debug();
	m_dstfilename = generatedstfilename(srcfilename);
	m_vencinfo = venc;
	m_aencinfo = aenc;
	m_estimated_packets = (int)(30.0*(double)duration)/1000.0;

	try
	{
		createdemuxer(srcfilename);
		createcodec(m_vencinfo, m_aencinfo);
		createmuxer(srcfilename);
#ifdef IMAGE_RESIZER_ON
		m_resizer.Create(venc.venc.width, venc.venc.height);
#endif

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
		StopMuxer();
		DestroyMuxer();
		m_demuxer.Destroy();
		m_vdecoder.Destroy();
		m_vencoder.Destroy();
#ifdef IMAGE_RESIZER_ON
		m_resizer.Destroy();
#endif
		m_adecoder.Destroy();
		m_aencoder.Destroy();

		m_bcreated = false;
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "TranscodingEngine", e.what());
	}
}

bool TranscodingEngine::IsCreated()
{
	return m_bcreated;
}


bool TranscodingEngine::feed_decoder_with_packet(CodecBase* decoder, int track_index, int& count)
{
	media_packet_h demux_packet;
	if(m_demuxer.IsEoS(track_index) == false)
	{
		if(m_demuxer.ReadSeample(track_index, &demux_packet))
		{
			++count;
			dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth demuxed packet", count);
			if(decoder == NULL)
			{
				media_packet_destroy(demux_packet);
			}
			else
			{
				if(!decoder->Input_Packets(demux_packet))
				{
					dlog_print(DLOG_ERROR, "TranscodingEngine", "while putting the %d th packet to the decoder", count);
					return false;
				}
			}
		}
	}
	return true;
}

bool TranscodingEngine::feed_encoder_with_packet(CodecBase* decoder, CodecBase* encoder, int& count)
{
	if(decoder == NULL || encoder == NULL)
		return false;
	media_packet_h decoded_packet;

	if(decoder->IsEoS()==false)
	{
		if(decoder->Get_Packets(decoded_packet))
		{
			++count;
			dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth decoded packet", count);

#ifdef IMAGE_RESIZER_ON
			media_packet_h resized_packet;
			if(m_resizer.Resize(decoded_packet, &resized_packet)==false)
			{
				dlog_print(DLOG_ERROR, "TranscodingEngine", "fail to resize");
			}
			media_packet_destroy(decoded_packet);
			if(!encoder->Input_Packets(resized_packet))
#else

			if(!encoder->Input_Packets(decoded_packet))
#endif
			{
				dlog_print(DLOG_ERROR, "TranscodingEngine", "while putting the %d th packet to the encoder", count);
				return false;
			}
		}
	}
	return true;
}

bool TranscodingEngine::feed_muxer_with_packet(CodecBase* encoder, int muxer_track_index, int& count)
{
	if(encoder == NULL)
		return false;
	media_packet_h encoded_packet;
	if(encoder->IsEoS()==false)
	{
		if(encoder->Get_Packets(encoded_packet))
		{
			++count;
			if(WriteSample(muxer_track_index, encoded_packet)==false)
			{
				media_packet_destroy(encoded_packet);
				return false;
			}
			media_packet_destroy(encoded_packet);
			dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth encoded packet", count);
		}
	}
	return true;
}


void TranscodingEngine::process_track(int track_index, int muxer_track_index, CodecBase* decoder, CodecBase* encoder, int counter[])
{
	if(feed_decoder_with_packet(decoder, track_index, counter[DEMUX_COUNTER])==false)
	{
		throw std::runtime_error("fail to feed_decoder_with_packet");
	}
	if(feed_encoder_with_packet(decoder, encoder, counter[DECODE_COUNTER])==false)
	{
		throw std::runtime_error("fail to feed_encoder_with_packet");
	}
	if(feed_muxer_with_packet(encoder, muxer_track_index, counter[ENCODE_COUNTER])==false)
	{
		throw std::runtime_error("fail to feed_muxer_with_packet");
	}
}

void TranscodingEngine::transcoding()
{
	int video_track_index = m_demuxer.GetVideoTrackIndex();
	int audio_track_index = m_demuxer.GetAudioTrackIndex();
	int video_counter[MAX_COUNTER] = {0,};
	int audio_counter[MAX_COUNTER] = {0,};

	while(1)
	{
		if(video_track_index != -1 && m_vencoder.IsEoS()==false)
		{
			process_track(video_track_index, m_muxer_video_track_index, (CodecBase*)&m_vdecoder, (CodecBase*)&m_vencoder, video_counter);
		}
		if(audio_track_index != -1 && m_aencoder.IsEoS()==false)
		{
			process_track(audio_track_index, m_muxer_audio_track_index, (CodecBase*)&m_adecoder, (CodecBase*)&m_aencoder, audio_counter);
		}

		m_progress_count = video_counter[ENCODE_COUNTER];

		if(m_vencoder.IsEoS() == true)
		{
			if(audio_track_index == -1)
				break;
			else
			{
				if(m_aencoder.IsEoS() == true)
					break;
			}
		}
		usleep(100000);
	}
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "the end of transcoding");
	if(video_track_index != -1)
		CloseTrack(m_muxer_video_track_index);
	if(audio_track_index != -1)
		CloseTrack(m_muxer_audio_track_index);

}
void TranscodingEngine::Start()
{
	try
	{
		m_demuxer.Prepare();
		transcoding();
		m_demuxer.Unprepare();
	}
	catch(const std::runtime_error& e)
	{
		throw e;
	}
}

void TranscodingEngine::Stop()
{

}

double TranscodingEngine::GetProgress()
{
	double progress = 0.0;
	if(m_estimated_packets != 0)
		progress = (double)m_progress_count/(double)m_estimated_packets;
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
	}
	catch(const std::runtime_error& e)
	{
		throw e;
	}
}
void TranscodingEngine::createmuxer(const char* srcfilename)
{
	if(!CreateMuxer(generatedstfilename(srcfilename), MEDIAMUXER_CONTAINER_FORMAT_MP4))
		throw std::runtime_error("fail to create muxer. please check muxer error");

	int video_track_index = m_demuxer.GetVideoTrackIndex();
	if((m_muxer_video_track_index = AddTrack(m_demuxer.GetMediaFormat(video_track_index))) == MEDIAMUXER_ERROR_INVALID_PARAMETER)
		throw std::runtime_error("fail to add a video track");

	int audio_track_index = m_demuxer.GetAudioTrackIndex();
	if(audio_track_index != -1)
	{
		if((m_muxer_audio_track_index = AddTrack(m_demuxer.GetMediaFormat(audio_track_index))) == MEDIAMUXER_ERROR_INVALID_PARAMETER)
			throw std::runtime_error("fail to add an audio track");
	}
	if(!StartMuxer())
		throw std::runtime_error("fail to start muxer");
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

		m_vdecoder.Create(vdec);
		m_vencoder.Create(venc);

		m_adecoder.Create(adec);
		m_aencoder.Create(aenc);

	}
	catch(const std::runtime_error& e)
	{
		throw e;
	}
}
const char* TranscodingEngine::generatedstfilename(const char* srcfilename)
{
	m_dstfilename = srcfilename;
	size_t dot_pos = m_dstfilename.find_last_of('.');
	m_dstfilename.insert(dot_pos, "_trans");
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "dest file name is %s", m_dstfilename.c_str());
	return m_dstfilename.c_str();
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



