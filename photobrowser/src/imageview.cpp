/*
 * imageview.cpp
 *
 *  Created on: Jul 7, 2016
 *      Author: Jason
 */

#include "base.h"
#include "imageview.h"

ImageView::ImageView()
:m_timer(NULL), m_image(NULL), m_rect(NULL), m_currentimageindex(0)
{

}
ImageView::~ImageView()
{

}

void ImageView::UpdateView()
{

}


const char* ImageView::getedcfilename()
{
	return "Slide Show";
}
//EVAS_CALLBACK_RESIZE

void ImageView::decorateview(Evas_Object* box)
{
	m_rect = evas_object_rectangle_add(evas_object_evas_get(box));
	if(m_rect == NULL)
	{
		dlog_print(DLOG_ERROR, "IMAGEVIEW", "fail to evas_object_rectangle_add");
		return;
	}
	evas_object_size_hint_weight_set(m_rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(m_rect, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_color_set(m_rect, 0, 0, 0, 255);
	evas_object_repeat_events_set(m_rect, EINA_TRUE);
	evas_object_event_callback_add(m_rect, EVAS_CALLBACK_RESIZE, ImageView::resize_event_cb , this);
	elm_box_pack_end(box, m_rect);
	evas_object_show(m_rect);


	m_image = elm_image_add(m_rect);
	if(m_image == NULL)
	{
		dlog_print(DLOG_ERROR, "IMAGEVIEW", "fail to evas_object_image_filled_add");
		return;
	}
	m_contents = getmodel()->GetContentsList();
	m_currentimageindex = getmodel()->GetSelectedIndex();
	elm_image_file_set(m_image, m_contents[m_currentimageindex].path.c_str(), NULL);
	evas_object_show(m_image);

	m_timer = ecore_timer_add(3.0, ImageView::timer_cb, (void*)this);
}

void ImageView::ontime()
{
	m_currentimageindex = (m_currentimageindex+1)%m_contents.size();
	elm_image_file_set(m_image, m_contents[m_currentimageindex].path.c_str(), NULL);
}

void ImageView::update_image()
{
	int x=0, y=0, w=0, h=0;
	evas_object_geometry_get(m_rect, &x, &y, &w, &h);
	dlog_print(DLOG_DEBUG, "IMAGEVIEW", "rect for image (%d, %d, %d, %d", x, y, w, h);

	evas_object_move(m_image, x, y);
	evas_object_resize(m_image, w, h);
}

void ImageView::destroyremains()
{
	ecore_timer_del(m_timer);
	evas_object_del(m_rect);
}

void ImageView::resize_event_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	ImageView* view = (ImageView*)data;
	view->update_image();
}

Eina_Bool ImageView::timer_cb(void *data)
{
	ImageView* view = (ImageView*)data;
	view->ontime();
	return EINA_TRUE;
}


