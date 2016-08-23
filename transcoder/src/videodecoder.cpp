/*
 * videodecoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */
#include "common/base.h"
#include "videodecoder.h"

VideoDecoder::VideoDecoder()
{


}
VideoDecoder::~VideoDecoder()
{

}
void VideoDecoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	iferror_throw(mediacodec_set_codec(mediacodec, codecinfo.vdec.codecid, MEDIACODEC_DECODER|MEDIACODEC_SUPPORT_TYPE_SW), "fail to mediacodec_set_codec");
	iferror_throw(mediacodec_set_vdec_info(mediacodec, codecinfo.vdec.width, codecinfo.vdec.height), "fail to mediacodec_set_vdec_info");
}

void VideoDecoder::destroy()
{

}

const char* VideoDecoder::getname()
{
	return "video decoder";
}
