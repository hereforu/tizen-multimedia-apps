/*
 * audioencoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */


#include "base.h"
#include "audioencoder.h"

AudioEncoder::AudioEncoder()
{


}
AudioEncoder::~AudioEncoder()
{

}
void AudioEncoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	AppTool::Iferror_throw(mediacodec_set_codec(mediacodec, codecinfo.venc.codecid, MEDIACODEC_ENCODER), "fail to mediacodec_set_codec");
	AppTool::Iferror_throw(mediacodec_set_aenc_info(mediacodec,
			codecinfo.aenc.samplerate, codecinfo.aenc.channel, codecinfo.aenc.bit, codecinfo.aenc.bitrate), "fail to mediacodec_set_aenc_info");

	media_format_create(&m_format);
	media_format_set_video_mime(m_format, (media_format_mimetype_e)(MEDIA_FORMAT_AUDIO | MEDIA_FORMAT_ENCODED | codecinfo.aenc.codecid));
	media_format_set_audio_channel(m_format, codecinfo.aenc.channel);
	media_format_set_audio_samplerate(m_format, codecinfo.aenc.samplerate);
	media_format_set_audio_bit(m_format, codecinfo.aenc.bit);
	media_format_set_audio_avg_bps(m_format, codecinfo.aenc.bitrate);

}
void AudioEncoder::destroy()
{

}


