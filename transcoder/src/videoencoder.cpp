/*
 * videoencoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */
#include "common/base.h"
#include "videoencoder.h"

VideoEncoder::VideoEncoder()
{


}
VideoEncoder::~VideoEncoder()
{

}
bool VideoEncoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = mediacodec_set_codec(mediacodec, codecinfo.venc.codecid, MEDIACODEC_ENCODER|MEDIACODEC_SUPPORT_TYPE_SW)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_codec [%d]", ret);
		return false;
	}
	if((ret = mediacodec_set_venc_info(mediacodec,
			codecinfo.venc.width, codecinfo.venc.height, codecinfo.venc.fps, codecinfo.venc.target_bits)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_set_venc_info [%d]", ret);
		return false;
	}
	dlog_print(DLOG_DEBUG, "CodecBase", "encoding option: width[%d], height[%d], fps[%d], target_bits[%d]", codecinfo.venc.width, codecinfo.venc.height, codecinfo.venc.fps, codecinfo.venc.target_bits);
	return true;
}


media_format_h VideoEncoder::create_format(const CodecInfo& codecinfo)
{
	media_format_h format = NULL;
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = media_format_create(&format)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to media_format_create [%d]", ret);
		return NULL;
	}
	media_format_set_video_mime(format, (media_format_mimetype_e)(MEDIA_FORMAT_VIDEO | MEDIA_FORMAT_ENCODED | codecinfo.venc.codecid));
	media_format_set_video_width(format, codecinfo.venc.width);
	media_format_set_video_height(format, codecinfo.venc.height);
	media_format_set_video_avg_bps(format, codecinfo.venc.target_bits);
	media_format_set_video_max_bps(format, codecinfo.venc.target_bits);
	media_format_set_video_frame_rate(format, codecinfo.venc.fps);
	dlog_print(DLOG_DEBUG, "CodecBase", "get encoder video format: width[%d], height[%d], fps[%d], target_bits[%d]", codecinfo.venc.width, codecinfo.venc.height, codecinfo.venc.fps, codecinfo.venc.target_bits);
	return format;
}

const char* VideoEncoder::getname()
{
	return "video encoder";
}






