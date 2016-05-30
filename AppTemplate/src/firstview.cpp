/*
 * FirstView.cpp
 *
 *  Created on: May 26, 2016
 *      Author: Jason
 */
#include "FirstView.h"
#include "multimediaapp.h"

FirstView::FirstView()
	:m_label(NULL)
{

}

FirstView::~FirstView()
{

}
void FirstView::showremains()
{
	evas_object_show(m_label);
	evas_object_show(m_btn);
}
const char* FirstView::getedcfilename()
{
	return "edje/firstview.edj";
}

bool FirstView::decorateview(Evas_Object* box)
{
	Evas_Object * label = elm_label_add(box);
	if(label == NULL)
		throw "fail to decorate the first view";
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, 0.5);
	elm_object_text_set(label, "This is the first view");
	elm_box_pack_end(box, label);
	evas_object_show(label);
	m_label = label;


	Evas_Object* btn = elm_button_add(box);
	elm_object_text_set(btn, "btn to move to the second view");
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", clicked_cb, this);
	elm_box_pack_end(box, btn);
	evas_object_show(btn);
	m_btn = btn;
	return true;
}

void FirstView::destroyremains()
{

}

void FirstView::clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	MultimediaApp::GetInstance()->MoveNextView();
}



