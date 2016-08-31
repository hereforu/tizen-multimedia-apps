/*
 * imageresizer.h
 *
 *  Created on: Jul 8, 2016
 *      Author: Jason
 */

#ifndef IMAGERESIZER_H_
#define IMAGERESIZER_H_

#include "sharedqueue.h"
#include <image_util.h>
#include "common/conditionalwaiter.h"
class ImageResizer
{
public:
	ImageResizer();
	~ImageResizer();

	void Create(int target_width, int target_height);
	void Destroy();

	bool Resize(media_packet_h packet, media_packet_h* resized_packet);

private:
	void print_packet_info(media_packet_h packet);
	void resize_completed(media_packet_h *dst, int error_code);
	static void resize_completed_cb(media_packet_h *dst, int error_code, void *user_data);

private:
	int m_target_width;
	int m_target_height;
	transformation_h m_handle;
	ConditionalWaiter* m_conditionalwaiter;
	media_packet_h m_result;



};


#endif /* IMAGERESIZER_H_ */
