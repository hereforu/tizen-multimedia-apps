/*
 * audioencoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */


#include "common/base.h"
#include "audioencoder.h"

AudioEncoder::AudioEncoder()
{


}
AudioEncoder::~AudioEncoder()
{

}
bool AudioEncoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = mediacodec_set_codec(mediacodec, codecinfo.aenc.codecid, MEDIACODEC_ENCODER|MEDIACODEC_SUPPORT_TYPE_SW)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_codec [%d]", ret);
		return false;
	}
	if((ret = mediacodec_set_aenc_info(mediacodec,
			codecinfo.aenc.samplerate, codecinfo.aenc.channel, codecinfo.aenc.bit, codecinfo.aenc.bitrate)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_aenc_info [%d]", ret);
		return false;
	}

	return true;
}

media_format_h AudioEncoder::create_format(const CodecInfo& codecinfo)
{
	media_format_h format = NULL;
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = media_format_create(&format)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to media_format_create [%d]", ret);
		return NULL;
	}
	media_format_set_audio_mime(format, (media_format_mimetype_e)(MEDIA_FORMAT_AUDIO | MEDIA_FORMAT_ENCODED | codecinfo.aenc.codecid));
	media_format_set_audio_channel(format, codecinfo.aenc.channel);
	media_format_set_audio_samplerate(format, codecinfo.aenc.samplerate);
	media_format_set_audio_bit(format, codecinfo.aenc.bit);
	media_format_set_audio_avg_bps(format, codecinfo.aenc.bitrate);
	return format;
}


const char* AudioEncoder::getname()
{
	return "audio encoder";
}



