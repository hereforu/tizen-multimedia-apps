/*
 * audiodecoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */


#include "base.h"
#include "audiodecoder.h"

AudioDecoder::AudioDecoder()
{


}
AudioDecoder::~AudioDecoder()
{

}
void AudioDecoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	AppTool::Iferror_throw(mediacodec_set_codec(mediacodec, codecinfo.venc.codecid, MEDIACODEC_ENCODER|MEDIACODEC_SUPPORT_TYPE_SW), "fail to mediacodec_set_codec");
	AppTool::Iferror_throw(mediacodec_set_adec_info(mediacodec,
			codecinfo.adec.samplerate, codecinfo.adec.channel, codecinfo.adec.bit), "fail to mediacodec_set_adec_info");
}
void AudioDecoder::destroy()
{

}
const char* AudioDecoder::getname()
{
	return "audio decoder";
}
