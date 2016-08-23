/*
 * videoencoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */
#include "common/base.h"
#include "videoencoder.h"

VideoEncoder::VideoEncoder()
:m_format(NULL)
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

	return create_format(codecinfo);

	media_format_create(&m_format);
	media_format_set_video_mime(m_format, (media_format_mimetype_e)(MEDIA_FORMAT_VIDEO | MEDIA_FORMAT_ENCODED | codecinfo.venc.codecid));
	media_format_set_video_width(m_format, codecinfo.venc.width);
	media_format_set_video_height(m_format, codecinfo.venc.height);
	media_format_set_video_avg_bps(m_format, codecinfo.venc.target_bits);
	media_format_set_video_max_bps(m_format, codecinfo.venc.target_bits);
	media_format_set_video_frame_rate(m_format, codecinfo.venc.fps);
}


bool VideoEncoder::create_format(const CodecInfo& codecinfo)
{
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = media_format_create(&m_format)) != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to media_format_create [%d]", ret);
		m_format = NULL;
		return false;
	}
	media_format_set_video_mime(m_format, (media_format_mimetype_e)(MEDIA_FORMAT_VIDEO | MEDIA_FORMAT_ENCODED | codecinfo.venc.codecid));
	media_format_set_video_width(m_format, codecinfo.venc.width);
	media_format_set_video_height(m_format, codecinfo.venc.height);
	media_format_set_video_avg_bps(m_format, codecinfo.venc.target_bits);
	media_format_set_video_max_bps(m_format, codecinfo.venc.target_bits);
	media_format_set_video_frame_rate(m_format, codecinfo.venc.fps);
	return true;
}


media_format_h VideoEncoder::GetMediaFormat()
{
	return m_format;
}
void VideoEncoder::destroy()
{
	if(m_format)
		media_format_unref(m_format);
}
const char* VideoEncoder::getname()
{
	return "video encoder";
}






