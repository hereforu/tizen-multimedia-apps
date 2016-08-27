/*
 * audiodecoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */


#include "common/base.h"
#include "audiodecoder.h"

AudioDecoder::AudioDecoder()
{


}
AudioDecoder::~AudioDecoder()
{

}
bool AudioDecoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = mediacodec_set_codec(mediacodec, codecinfo.venc.codecid, MEDIACODEC_DECODER|MEDIACODEC_SUPPORT_TYPE_SW)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_codec [%d]", ret);
		return false;
	}
	if((ret = mediacodec_set_adec_info(mediacodec,
			codecinfo.adec.samplerate, codecinfo.adec.channel, codecinfo.adec.bit)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_adec_info [%d]", ret);
		return false;
	}
	return true;
}
media_format_h AudioDecoder::create_format(const CodecInfo& codecinfo)
{
	media_format_h format = NULL;
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = media_format_create(&format)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to media_format_create [%d]", ret);
		return NULL;
	}
	media_format_set_audio_mime(format, (media_format_mimetype_e)(MEDIA_FORMAT_AUDIO | MEDIA_FORMAT_ENCODED | codecinfo.adec.codecid));
	media_format_set_audio_channel(format, codecinfo.adec.channel);
	media_format_set_audio_samplerate(format, codecinfo.adec.samplerate);
	media_format_set_audio_bit(format, codecinfo.adec.bit);
	//media_format_set_audio_avg_bps(format, codecinfo.aenc.bitrate);
	return format;
}
void AudioDecoder::destroy()
{

}
const char* AudioDecoder::getname()
{
	return "audio decoder";
}
