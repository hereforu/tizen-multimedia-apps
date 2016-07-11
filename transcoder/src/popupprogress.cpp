/*
 * popupprogress.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: Jason
 */

#include "popupprogress.h"
#include <stdexcept>

PopupProgress::PopupProgress()
:m_cancelbtn(NULL), m_pb(NULL), m_popup(NULL), m_parent(NULL)
{

}

PopupProgress::~PopupProgress()
{

}
void PopupProgress::Create(Evas_Object* parent, Porgress_Cancel_Cb cancel_cb, void* data)
{
	try
	{
		m_cancelcb = cancel_cb;
		m_data = data;
		m_popup = createpopup(parent);
		m_pb = createpb(m_popup);
		m_cancelbtn = createcancelbtn(m_popup);
		elm_object_part_content_set(m_popup, "default", m_pb);
		elm_object_part_content_set(m_popup, "button2", m_cancelbtn);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "InfoView", e.what());
		throw std::runtime_error(std::string("fail to create popup list control")+e.what());
	}
	m_parent = parent;
}

void PopupProgress::Destroy()
{
	evas_object_del(m_pb);
	evas_object_del(m_cancelbtn);
	evas_object_del(m_popup);
}

void PopupProgress::Popup()
{
	 evas_object_show(m_popup);
}
void PopupProgress::Hide()
{
	evas_object_hide(m_popup);
}

void PopupProgress::SetValue(double value)
{
	elm_progressbar_value_set(m_pb, value);
}

Evas_Object* PopupProgress::createpopup(Evas_Object* parent)
{
	Evas_Object* popup = elm_popup_add(parent);
	if(popup==NULL)
	{
		throw std::runtime_error("fail to create popup");
	}
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	return popup;
}
Evas_Object* PopupProgress::createpb(Evas_Object* parent)
{
	Evas_Object* pb = elm_progressbar_add(parent);
	if(pb==NULL)
	{
		throw std::runtime_error("fail to create Progress");
	}
//	evas_object_size_hint_weight_set(pb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//	evas_object_size_hint_align_set(pb, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_style_set(pb, "default");
	elm_object_text_set(pb, "Transcoding...");
	elm_progressbar_value_set(pb, 0.0);
	return pb;
}
Evas_Object* PopupProgress::createcancelbtn(Evas_Object* parent)
{
	Evas_Object* btn = elm_button_add(parent);
	if(btn==NULL)
	{
		throw std::runtime_error("fail to create Progress");
	}
	std::string resource_path = app_get_resource_path();
	resource_path += "images/cancel.png";
	Evas_Object *ic;
	ic = elm_icon_add(btn);
	elm_image_file_set(ic, resource_path.c_str(), NULL);
	elm_object_part_content_set(btn, "icon", ic);
	elm_object_style_set(btn,"circle");
	evas_object_smart_callback_add(btn, "clicked", PopupProgress::clicked_cancel_cb, (void*)this);
	return btn;
}
void PopupProgress::cancel()
{
	//ToDO::
#if 0
	Hide();
	m_cancelcb(m_data);
#endif
}
void PopupProgress::clicked_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
	PopupProgress* popup = (PopupProgress*)data;
	popup->cancel();
}


