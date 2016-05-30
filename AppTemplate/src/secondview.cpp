/*
 * secondview.cpp
 *
 *  Created on: May 27, 2016
 *      Author: Jason
 */

#include "SecondView.h"
#include "multimediaapp.h"

SecondView::SecondView()
:m_label(NULL)
{

}

SecondView::~SecondView()
{

}
void SecondView::showremains()
{

}
const char* SecondView::getedcfilename()
{
	return "edje/secondview.edj";
}

bool SecondView::decorateview(Evas_Object* box)
{
	Evas_Object * label = elm_label_add(box);
	if(label == NULL)
		throw "fail to decorate the second view";
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, 0.5);
	elm_object_text_set(label, "This is the second view");
	elm_box_pack_end(box, label);
	evas_object_show(label);
	m_label = label;


	Evas_Object* btn = elm_button_add(box);
	elm_object_text_set(btn, "btn to move to the first view");
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", clicked_cb_in_secondview, this);
	elm_box_pack_end(box, btn);
	evas_object_show(btn);

	return true;
}

void SecondView::clicked_cb_in_secondview(void *data, Evas_Object *obj, void *event_info)
{
	MultimediaApp::GetInstance()->MovePrevView();
}

void SecondView::destroyremains()
{

}




