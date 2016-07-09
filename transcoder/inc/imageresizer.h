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
#include "eina_lock.h"

class ImageResizer
{
public:
	ImageResizer();
	~ImageResizer();

	void Create(int target_width, int target_height);
	void Destroy();

	bool Resize(media_packet_h packet, media_packet_h* resized_packet);

private:
	void resize_completed(media_packet_h *dst, int error_code);
	static void resize_completed_cb(media_packet_h *dst, int error_code, void *user_data);

private:
	transformation_h m_handle;
	Eina_Lock m_mutex;
	Eina_Condition m_cond;
	media_packet_h* m_result;



};


#endif /* IMAGERESIZER_H_ */
