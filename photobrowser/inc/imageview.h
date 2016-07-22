/*
 * imageview.h
 *
 *  Created on: Jul 7, 2016
 *      Author: Jason
 */

#ifndef IMAGEVIEW_H_
#define IMAGEVIEW_H_


#include "common/view.h"
#include "photobrowsermodel.h"
#include "exif.h"

class ImageView : public View
{
public:
	ImageView();
	~ImageView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();



private:
	void ontime();
	void update_image();
	static void resize_event_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
	static Eina_Bool timer_cb(void *data);
private:
	Ecore_Timer* m_timer;
	Evas_Object* m_image;
	Evas_Object* m_rect;
	int m_currentimageindex;
	std::vector<MediaContentItem> m_contents;
};


#endif /* IMAGEVIEW_H_ */
