/*
 * imageresizer.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: Jason
 */

#include "base.h"
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
		throw std::runtime_error(std::string("fail to image_util_transform_create: ")+AppTool::ToString(ret));
	ret = image_util_transform_set_resolution(m_handle, target_width, target_height);
}

void ImageResizer::Destroy()
{
	image_util_transform_destroy(m_handle);
	m_handle = NULL;
	eina_condition_free(&m_cond);
	eina_lock_free(&m_mutex);

}

bool ImageResizer::Resize(media_packet_h packet, media_packet_h* resized_packet)
{
	int ret = image_util_transform_run(m_handle, packet, ImageResizer::resize_completed_cb, (void*)this);
	dlog_print(DLOG_DEBUG, "ImageResizer", "image_util_transform_run[%d]", ret);
	if(ret != IMAGE_UTIL_ERROR_NONE)
	{
		return false;
	}
	eina_condition_wait(&m_cond);
	*resized_packet = *m_result;
	dlog_print(DLOG_DEBUG, "ImageResizer", "image_util_transform_run signaled");
	return true;
}

void ImageResizer::resize_completed(media_packet_h *dst, int error_code)
{
	m_result = dst;
	dlog_print(DLOG_DEBUG, "ImageResizer", "condition signal");
	eina_condition_signal(&m_cond);
}

void ImageResizer::resize_completed_cb(media_packet_h *dst, int error_code, void *user_data)
{
	ImageResizer* resizer = (ImageResizer*)user_data;
	resizer->resize_completed(dst, error_code);
}


