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

#define IMAGE_RESIZER_ON



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
		createvideocodec(m_vencinfo);
		if(isaudioavailable())
		{
			createaudiocodec(m_aencinfo);
		}
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
		m_muxer.Destroy();
		m_demuxer.Destroy();
		m_vdecoder.Destroy();
		m_vencoder.Destroy();
		if(isaudioavailable())
		{
			m_adecoder.Destroy();
			m_aencoder.Destroy();
		}
#ifdef IMAGE_RESIZER_ON
		m_resizer.Destroy();
#endif
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


bool TranscodingEngine::feed_decoder_with_packet(CodecBase* decoder, int track_index, int& count, unsigned int& pts)
{
	media_packet_h demux_packet;
	if(m_demuxer.IsEoS(track_index) == false)
	{
		if(m_demuxer.ReadSeample(track_index, &demux_packet))
		{
			++count;
			uint64_t temp_pts = 0;
			media_packet_get_pts(demux_packet, &temp_pts);
			pts = (unsigned int)(temp_pts/1000000);
			dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth demuxed packet [pts:%llu]", count, pts);
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
			bool isvideo = false;
			if(media_packet_is_video(decoded_packet, &isvideo) == MEDIA_PACKET_ERROR_NONE && isvideo==true)
			{
				media_packet_h resized_packet;
				if(m_resizer.Resize(decoded_packet, &resized_packet)==true)
				{
					media_packet_destroy(decoded_packet);
					decoded_packet = resized_packet;
				}
				else
				{
					dlog_print(DLOG_ERROR, "TranscodingEngine", "fail to resize");
				}
			}
#endif
			if(!encoder->Input_Packets(decoded_packet))
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
			if(m_muxer.WriteSample(muxer_track_index, encoded_packet)==false)
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


void TranscodingEngine::process_track(int track_index, int muxer_track_index, CodecBase* decoder, CodecBase* encoder, int counter[], unsigned int& pts)
{
	if(feed_decoder_with_packet(decoder, track_index, counter[DEMUX_COUNTER], pts)==false)
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

bool TranscodingEngine::isaudioavailable()
{
#ifdef AUDIO_EXCLUDED
	return false;
#else
	return (m_demuxer.GetAudioTrackIndex() != -1)?true:false;
#endif
}

void TranscodingEngine::transcoding()
{
	int video_counter[MAX_COUNTER] = {0,};
	int audio_counter[MAX_COUNTER] = {0,};
	int iteration = 0;
	unsigned int video_pts = 0, audio_pts = 0;
	while(1)
	{
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth iteration for transcoding", iteration++);
		if(m_vencoder.IsEoS()==false)
		{
			process_track(m_demuxer.GetVideoTrackIndex(), m_muxer_video_track_index, (CodecBase*)&m_vdecoder, (CodecBase*)&m_vencoder, video_counter, video_pts);
		}
		if(isaudioavailable() && m_aencoder.IsEoS()==false)
		{
			process_track(m_demuxer.GetAudioTrackIndex(), m_muxer_audio_track_index, (CodecBase*)&m_adecoder, (CodecBase*)&m_aencoder, audio_counter, audio_pts);
		}
		if(isaudioavailable() && m_aencoder.IsEoS()==false)
		{
			if(audio_pts < video_pts)
				process_track(m_demuxer.GetAudioTrackIndex(), m_muxer_audio_track_index, (CodecBase*)&m_adecoder, (CodecBase*)&m_aencoder, audio_counter, audio_pts);
		}

		m_progress_count = video_counter[ENCODE_COUNTER];

		if(m_vencoder.IsEoS() == true)
		{
			if(isaudioavailable())
			{
				if(m_aencoder.IsEoS() == true)
					break;
			}
			else
				break;
		}
		usleep(100000);
	}
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "the end of transcoding");

	m_muxer.CloseTrack(m_muxer_video_track_index);
	if(isaudioavailable())
		m_muxer.CloseTrack(m_muxer_audio_track_index);
	usleep(1000000);
}
void TranscodingEngine::Start()
{
	m_demuxer.Start();
	m_muxer.Start();
	transcoding();
	m_muxer.Stop();
	m_demuxer.Stop();
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


void TranscodingEngine::createdemuxer(const char* srcfilename)
{
	m_demuxer.Create(srcfilename);
	m_demuxer.ExtractTrackinfo();
}

void TranscodingEngine::createmuxer(const char* srcfilename)
{
	m_muxer.Create(generatedstfilename(srcfilename), MEDIAMUXER_CONTAINER_FORMAT_MP4);
	m_muxer_video_track_index = m_muxer.AddTrack(m_demuxer.GetMediaFormat(m_demuxer.GetVideoTrackIndex()));
	if(isaudioavailable())
		m_muxer_audio_track_index = m_muxer.AddTrack(m_demuxer.GetMediaFormat(m_demuxer.GetAudioTrackIndex()));
}

void TranscodingEngine::createvideocodec(CodecInfo& venc)
{
	CodecInfo vdec;
	if(m_demuxer.GetVideoDecInfo(vdec)==false)
	{
		throw std::runtime_error("fail to get video info from demuxer");
	}
	if(venc.venc.codecid == 0)//ORIGINAL_FEATURE
		venc.venc.codecid = vdec.vdec.codecid;
	m_vdecoder.Create(vdec);
	m_vencoder.Create(venc);
	dlog_print(DLOG_INFO, "TranscodingEngine", "video decode: codecid:%d, width:%d, height:%d", vdec.vdec.codecid, vdec.vdec.width, vdec.vdec.height);
	dlog_print(DLOG_INFO, "TranscodingEngine", "video encode: codecid:%d, width:%d, height:%d, fps:%d, bitrate:%d"
			, venc.venc.codecid, venc.venc.width, venc.venc.height, venc.venc.fps, venc.venc.target_bits);
}
void TranscodingEngine::createaudiocodec(CodecInfo& aenc)
{
	CodecInfo adec;
	if(m_demuxer.GetAudioDecInfo(adec)==false)
	{
		throw std::runtime_error("fail to get audio info from demuxer");
	}
	if(aenc.aenc.codecid == 0)//ORIGINAL_FEATURE
		aenc.aenc.codecid = adec.adec.codecid;
	aenc.aenc.bit = adec.adec.bit;
	aenc.aenc.channel = adec.adec.channel;
	aenc.aenc.samplerate = adec.adec.samplerate;
	aenc.aenc.bitrate = aenc.aenc.bit*aenc.aenc.channel*aenc.aenc.samplerate;
	m_adecoder.Create(adec);
	m_aencoder.Create(aenc);
	dlog_print(DLOG_INFO, "TranscodingEngine", "audio decode: codecid:%d, bit:%d, channel:%d, samplerate:%d"
					, adec.adec.codecid, adec.adec.bit, adec.adec.channel, adec.adec.samplerate);
	dlog_print(DLOG_INFO, "TranscodingEngine", "audio encode: codecid:%d, bit:%d, channel:%d, samplerate:%d, bitrate:%d"
							, aenc.aenc.codecid, aenc.aenc.bit, aenc.aenc.channel, aenc.aenc.samplerate, aenc.aenc.bitrate);
}

const char* TranscodingEngine::generatedstfilename(const char* srcfilename)
{
	m_dstfilename = srcfilename;
	size_t dot_pos = m_dstfilename.find_last_of('.');
	m_dstfilename.insert(dot_pos, "_trans");
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "dest file name is %s", m_dstfilename.c_str());
	return m_dstfilename.c_str();
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




