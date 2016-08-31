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
#include <device/power.h>





TranscodingEngine::TranscodingEngine()
:m_demuxer(NULL),m_muxer(NULL),m_resizer(NULL),
 m_progress_count(0), m_estimated_packets(0),
 m_muxer_video_track_index(-1), m_muxer_audio_track_index(-1),
 m_bcanceled(false)
{
	for(int i = 0; i < MAX_CODEC; ++i)
		m_codec[i] = NULL;
}

TranscodingEngine::~TranscodingEngine()
{
	unprepare();
}

void TranscodingEngine::Transcoding(const char* srcfilename, unsigned int duration, const CodecInfo& venc, const CodecInfo& aenc)
{
	device_power_request_lock(POWER_LOCK_CPU, 0);
	prepare(srcfilename, duration, venc, aenc);
	m_demuxer->Start();
	m_muxer->Start();
	transcoding();
	m_muxer->Stop();
	m_demuxer->Stop();
	unprepare();
	device_power_release_lock(POWER_LOCK_CPU);
}

void TranscodingEngine::Cancel()
{
	m_bcanceled = true;
}

double TranscodingEngine::GetProgress()
{
	double progress = 0.0;
	if(m_estimated_packets != 0)
		progress = (double)m_progress_count/(double)m_estimated_packets;
	return progress;
}
const char* TranscodingEngine::GetDstFileName()
{
	return m_dstfilename.c_str();
}
bool TranscodingEngine::IsCanceled()
{
	return m_bcanceled;
}
void TranscodingEngine::prepare(const char* srcfilename, unsigned int duration, const CodecInfo& venc, const CodecInfo& aenc)
{
	print_errorcode_for_debug();
	m_dstfilename = generatedstfilename(srcfilename);
	m_estimated_packets = (int)(30.0*(double)duration)/1000.0;
	create_demuxer(srcfilename);
	CodecInfo option[MAX_CODEC];
	option[VIDEO_ENCODER] = venc;
	option[AUDIO_ENCODER] = aenc;
	fill_and_get_codec_info(option[VIDEO_DECODER], option[AUDIO_DECODER], option[VIDEO_ENCODER], option[AUDIO_ENCODER]);
	for(int i = 0; i < MAX_CODEC; ++i)
		create_codec(i, option[i]);
	create_muxer(srcfilename);
	if(option[VIDEO_ENCODER].venc.width != option[VIDEO_DECODER].vdec.width || option[VIDEO_ENCODER].venc.height != option[VIDEO_DECODER].vdec.height)
		create_resizer(option[VIDEO_ENCODER].venc.width, option[VIDEO_ENCODER].venc.height);
}

void TranscodingEngine::unprepare()
{
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "enter into destroy");
	SAFE_DELETE(m_muxer);
	SAFE_DELETE(m_demuxer);
	SAFE_DELETE(m_resizer);
	for(int i = 0; i < MAX_CODEC; ++i)
	{
		SAFE_DELETE(m_codec[i]);
	}
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "exit from destroy");
}


bool TranscodingEngine::feed_decoder_with_packet(CodecBase* decoder, int track_index, int& count, unsigned int& pts)
{
	media_packet_h demux_packet = NULL;
	if(m_demuxer->IsEoS(track_index) == true)
		return true;
	if(m_demuxer->ReadSeample(track_index, &demux_packet))
	{
		++count;
		pts = get_pts_in_msec(demux_packet);
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth demuxed packet [pts:%u]", count, pts);
		if(!decoder->InsertPacket(demux_packet))
		{
			dlog_print(DLOG_ERROR, "TranscodingEngine", "while putting the %d th packet to the decoder", count);
			return false;
		}
	}
	return true;
}

unsigned int TranscodingEngine::get_pts_in_msec(media_packet_h packet)
{
	uint64_t temp_pts = 0;
	media_packet_get_pts(packet, &temp_pts);
	return (unsigned int)(temp_pts/1000000);
}

bool TranscodingEngine::feed_encoder_with_packet(CodecBase* decoder, CodecBase* encoder, int& count)
{
	media_packet_h decoded_packet = NULL;
	if(decoder->IsEoS()==true)
		return true;
	if(decoder->GetPacket(decoded_packet))
	{
		++count;
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth decoded packet", count);
		if(m_resizer) //there will be no resizer object if the resolutions of a source and a target video are identical
		{
			if(resize_resolution_if_image(&decoded_packet)==false)
			{
				dlog_print(DLOG_ERROR, "TranscodingEngine", "while resizing the %d th packet", count);
				return false;
			}
		}
		if(encoder->InsertPacket(decoded_packet)==false)
		{
			dlog_print(DLOG_ERROR, "TranscodingEngine", "while putting the %d th packet to the encoder", count);
			return false;
		}
	}
	return true;
}

//it returns false when the resize function has been failed!
bool TranscodingEngine::resize_resolution_if_image(media_packet_h* packet)
{
	bool isvideo = false;
	if(media_packet_is_video(*packet, &isvideo) == MEDIA_PACKET_ERROR_NONE && isvideo==true)
	{
		media_packet_h resized_packet;
		if(m_resizer->Resize(*packet, &resized_packet)==true)
		{
			media_packet_destroy(*packet);
			*packet = resized_packet;
		}
		else
		{
			dlog_print(DLOG_ERROR, "TranscodingEngine", "fail to resize");
			return false;
		}
	}
	return true;
}

bool TranscodingEngine::feed_muxer_with_packet(CodecBase* encoder, int muxer_track_index, int& count)
{
	bool bret = true;
	if(encoder == NULL)
		return false;
	media_packet_h encoded_packet;
	if(encoder->IsEoS()==false)
	{
		if(encoder->GetPacket(encoded_packet))
		{
			++count;
			bret = m_muxer->WriteSample(muxer_track_index, encoded_packet);
			media_packet_destroy(encoded_packet);
			dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth encoded packet", count);
		}
	}
	return bret;
}


void TranscodingEngine::process_track(int track_index, int muxer_track_index, CodecBase* decoder, CodecBase* encoder, int counter[], unsigned int& pts)
{
	if(encoder->IsEoS()==true)
		return;
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


void TranscodingEngine::transcoding()
{
	int video_counter[MAX_COUNTER] = {0,};
	int audio_counter[MAX_COUNTER] = {0,};
	int iteration = 0;
	unsigned int video_pts = 0, audio_pts = 0;
	m_bcanceled = false;
	while(!m_bcanceled)
	{
		dlog_print(DLOG_DEBUG, "TranscodingEngine", "%dth iteration for transcoding", iteration++);
		process_track(m_demuxer->GetVideoTrackIndex(), m_muxer_video_track_index, m_codec[VIDEO_DECODER], m_codec[VIDEO_ENCODER], video_counter, video_pts);
		do
		{
			process_track(m_demuxer->GetAudioTrackIndex(), m_muxer_audio_track_index, m_codec[AUDIO_DECODER], m_codec[AUDIO_ENCODER], audio_counter, audio_pts);
		}while(audio_pts <= video_pts && m_codec[AUDIO_ENCODER]->IsEoS()==false);
		if(m_codec[VIDEO_ENCODER]->IsEoS() == true && m_codec[AUDIO_ENCODER]->IsEoS() == true)
		{
			break;
		}
		m_progress_count = video_counter[ENCODE_COUNTER];
		usleep(500000);
	}
	dlog_print(DLOG_DEBUG, "TranscodingEngine", "the end of transcoding");
	m_muxer->CloseTrack(m_muxer_video_track_index);
	m_muxer->CloseTrack(m_muxer_audio_track_index);
	usleep(1000000);
}


void TranscodingEngine::create_demuxer(const char* srcfilename)
{
	std::auto_ptr<Demuxer> demuxer(new Demuxer);
	demuxer->Create(srcfilename);
	demuxer->ExtractTrackinfo();
	m_demuxer = demuxer.release();
}

void TranscodingEngine::create_muxer(const char* srcfilename)
{
	std::auto_ptr<Muxer> muxer(new Muxer);
	muxer->Create(m_dstfilename.c_str(), MEDIAMUXER_CONTAINER_FORMAT_MP4);
	m_muxer_video_track_index = muxer->AddTrack(m_codec[VIDEO_ENCODER]->GetMediaFormat());
	m_muxer_audio_track_index = muxer->AddTrack(m_codec[AUDIO_ENCODER]->GetMediaFormat());
	m_muxer = muxer.release();
}
void TranscodingEngine::create_resizer(int target_width, int target_height)
{
	std::auto_ptr<ImageResizer> resizer(new ImageResizer);
	resizer->Create(target_width, target_height);
	m_resizer = resizer.release();
}



void TranscodingEngine::create_codec(int codectype, const CodecInfo& codecinfo)
{
	CodecBase* p = NULL;
	switch(codectype)
	{
		case VIDEO_DECODER:
			p = (CodecBase*)new VideoDecoder;
			break;
		case VIDEO_ENCODER:
			p = (CodecBase*)new VideoEncoder;
			break;
		case AUDIO_DECODER:
			p = (CodecBase*)new AudioDecoder;
			break;
		case AUDIO_ENCODER:
			p = (CodecBase*)new AudioEncoder;
			break;
		default:
			throw std::runtime_error("invalid codec type");
			break;
	}
	std::auto_ptr<CodecBase> codec(p);
	codec->Create(codecinfo);
	m_codec[codectype] = codec.release();
}


void TranscodingEngine::fill_and_get_codec_info(CodecInfo& vdec, CodecInfo& adec, CodecInfo& venc, CodecInfo& aenc)
{
	if(m_demuxer->GetVideoDecInfo(vdec)==false)
	{
		throw std::runtime_error("fail to get video info from demuxer");
	}
	if(m_demuxer->GetAudioDecInfo(adec)==false)
	{
		throw std::runtime_error("fail to get audio info from demuxer");
	}
	if(venc.venc.codecid == 0)//ORIGINAL_FEATURE
		venc.venc.codecid = vdec.vdec.codecid;
	if(aenc.aenc.codecid == 0)//ORIGINAL_FEATURE
		aenc.aenc.codecid = adec.adec.codecid;
	aenc.aenc.bit = adec.adec.bit;
	aenc.aenc.channel = adec.adec.channel;
	aenc.aenc.samplerate = adec.adec.samplerate;
	aenc.aenc.bitrate = aenc.aenc.bit*aenc.aenc.channel*aenc.aenc.samplerate;
}

const char* TranscodingEngine::generatedstfilename(const char* srcfilename)
{
	int i = 0;
	while(1)
	{
		m_dstfilename = srcfilename;
		size_t dot_pos = m_dstfilename.find_last_of('.');
		std::string postfix = "_trans";
		if(i > 0)
		{
			postfix += to_string<int>(i);
		}
		m_dstfilename.insert(dot_pos, postfix.c_str());
		if(access(m_dstfilename.c_str(), F_OK) == -1)
		{
			break;
		}
		++i;
	}
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




