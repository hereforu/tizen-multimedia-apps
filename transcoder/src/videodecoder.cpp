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
bool VideoDecoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = mediacodec_set_codec(mediacodec, codecinfo.vdec.codecid, MEDIACODEC_DECODER|MEDIACODEC_SUPPORT_TYPE_SW)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_codec [%d]", ret);
		return false;
	}
	if((ret = mediacodec_set_vdec_info(mediacodec,
			codecinfo.vdec.width, codecinfo.vdec.height)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_vdec_info [%d]", ret);
		return false;
	}
	return true;
}

media_format_h VideoDecoder::create_format(const CodecInfo& codecinfo)
{
	media_format_h format = NULL;
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = media_format_create(&format)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to media_format_create [%d]", ret);
		return NULL;
	}
	media_format_set_video_mime(format, (media_format_mimetype_e)(MEDIA_FORMAT_VIDEO | MEDIA_FORMAT_ENCODED | codecinfo.vdec.codecid));
	media_format_set_video_width(format, codecinfo.vdec.width);
	media_format_set_video_height(format, codecinfo.vdec.height);
//	media_format_set_video_avg_bps(format, codecinfo.venc.target_bits);
//	media_format_set_video_max_bps(format, codecinfo.venc.target_bits);
//	media_format_set_video_frame_rate(format, codecinfo.venc.fps);
	return format;
}



const char* VideoDecoder::getname()
{
	return "video decoder";
}
