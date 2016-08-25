/*
 * imageresizer.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "imageresizer.h"
#include <stdexcept>

ImageResizer::ImageResizer()
:m_handle(NULL), m_result(NULL)
{

}

ImageResizer::~ImageResizer()
{

}

void ImageResizer::Create(int target_width, int target_height)
{
	if(eina_lock_new(&m_mutex)==EINA_FALSE)
	{
		throw std::runtime_error("fail to create eina_lock_new");
	}
	if(eina_condition_new(&m_cond, &m_mutex)==EINA_FALSE)
	{
		throw std::runtime_error("fail to create eina_lock_new");
	}

	int ret = image_util_transform_create(&m_handle);
	if(ret != IMAGE_UTIL_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to image_util_transform_create: ")+to_string(ret));
	}
	ret = image_util_transform_set_resolution(m_handle, target_width, target_height);
}

void ImageResizer::Destroy()
{
	dlog_print(DLOG_DEBUG, "ImageResizer", "enter into destroy");
	image_util_transform_destroy(m_handle);
	m_handle = NULL;
	eina_condition_free(&m_cond);
	eina_lock_free(&m_mutex);
	dlog_print(DLOG_DEBUG, "ImageResizer", "exit from destroy");
}

bool ImageResizer::Resize(media_packet_h packet, media_packet_h* resized_packet)
{
//	dlog_print(DLOG_DEBUG, "ImageResizer", "========================original packet===========================");
//	print_packet_info(packet);
	bool is_eos = false;
	media_packet_is_end_of_stream(packet, &is_eos);
	uint64_t	pts = 0;
	media_packet_get_pts(packet, &pts);

	int ret = image_util_transform_run(m_handle, packet, ImageResizer::resize_completed_cb, (void*)this);
	dlog_print(DLOG_DEBUG, "ImageResizer", "image_util_transform_run[%d]", ret);
	if(ret != IMAGE_UTIL_ERROR_NONE)
	{
		return false;
	}
	eina_condition_wait(&m_cond);
	*resized_packet = m_result;
//	dlog_print(DLOG_DEBUG, "ImageResizer", "========================resized packet=================[%p], [%p]", *resized_packet, m_result);
	if(is_eos)
	{
		media_packet_set_flags(*resized_packet, MEDIA_PACKET_END_OF_STREAM);
	}
	media_packet_set_pts(*resized_packet, pts);
//	print_packet_info(*resized_packet);
	dlog_print(DLOG_DEBUG, "ImageResizer", "image_util_transform_run signaled");
	return true;
}

void ImageResizer::resize_completed(media_packet_h *dst, int error_code)
{
	m_result = *dst;
	dlog_print(DLOG_DEBUG, "ImageResizer", "condition signal dst[%p], error_code[%d]", *dst, error_code);
	eina_condition_signal(&m_cond);
}

void ImageResizer::resize_completed_cb(media_packet_h *dst, int error_code, void *user_data)
{
	dlog_print(DLOG_DEBUG, "ImageResizer", "resize_completed_cb is called [%p]", *dst );
	ImageResizer* resizer = (ImageResizer*)user_data;
	resizer->resize_completed(dst, error_code);
}

void ImageResizer::print_packet_info(media_packet_h packet)
{
	uint64_t	pts = 0;
	media_packet_get_pts(packet, &pts);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_get_pts [%llu]", pts);

	uint64_t	dts = 0;
	media_packet_get_dts(packet, &dts);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_get_dts [%llu]", dts);

	uint64_t	duration = 0;
	media_packet_get_duration(packet, &duration);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_get_duration [%llu]", dts);

	media_format_h fmt = NULL;
	media_format_mimetype_e mimetype;
	int width = 0, height = 0, avg_bps=0, max_bps = 0;
	int ret = media_packet_get_format(packet, &fmt);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_get_format: [%p] [%d]=====", fmt, ret);
	media_format_get_video_info(fmt, &mimetype, &width, &height, &avg_bps, &max_bps);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_format_get_video_info: vidoe mimetype: %x, w:%d, h:%d, avg_bps:%d, max_bps:%d =====", (int)mimetype, width, height, avg_bps, max_bps);
	media_format_unref(fmt);

	bool codec_config = false;
	media_packet_is_codec_config(packet, &codec_config);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_is_codec_config [%d]", (int)codec_config);

	bool is_encoded = false;
	media_packet_is_encoded(packet, &is_encoded);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_is_encoded [%d]", (int)is_encoded);

	bool is_eos = false;
	media_packet_is_end_of_stream(packet, &is_eos);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_is_end_of_stream [%d]", (int)is_eos);

	bool is_raw = false;
	media_packet_is_raw(packet, &is_raw);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_is_raw [%d]", (int)is_raw);

	bool is_sync = false;
	media_packet_is_sync_frame(packet, &is_sync);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_is_sync_frame [%d]", (int)is_sync);

	bool is_video = false;
	media_packet_is_video(packet, &is_video);
	dlog_print(DLOG_DEBUG, "ImageResizer", "media_packet_is_video [%d]", (int)is_video);


}


