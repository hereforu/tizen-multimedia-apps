/*
 * videoencoder.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */
#include "base.h"
#include "videoencoder.h"

VideoEncoder::VideoEncoder()
{


}
VideoEncoder::~VideoEncoder()
{

}
void VideoEncoder::create(mediacodec_h mediacodec, const CodecInfo& codecinfo)
{
	AppTool::Iferror_throw(mediacodec_set_codec(mediacodec, codecinfo.venc.codecid, MEDIACODEC_ENCODER), "fail to mediacodec_set_codec");
	AppTool::Iferror_throw(mediacodec_set_venc_info(mediacodec,
			codecinfo.venc.width, codecinfo.venc.height, codecinfo.venc.fps, codecinfo.venc.target_bits), "fail to mediacodec_set_venc_info");

	media_format_create(&m_format);
	media_format_set_video_mime(m_format, (media_format_mimetype_e)(MEDIA_FORMAT_VIDEO | MEDIA_FORMAT_ENCODED | codecinfo.venc.codecid));
	media_format_set_video_width(m_format, codecinfo.venc.width);
	media_format_set_video_height(m_format, codecinfo.venc.height);
	media_format_set_video_avg_bps(m_format, codecinfo.venc.target_bits);
	media_format_set_video_max_bps(m_format, codecinfo.venc.target_bits);
}
void VideoEncoder::destroy()
{

}






