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
void AudioDecoder::destroy()
{

}
const char* AudioDecoder::getname()
{
	return "audio decoder";
}
