/*
 * view.cpp
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "apptemplate.h"
#include "view.h"

View::View()
	:m_box(NULL), m_layout(NULL), m_naviframe(NULL), m_conformant(NULL), m_Naviitem(NULL)
{

}
View::~View()
{
	//how to delete layout??
}

bool View::CreateView(Evas_Object* naviframe, Evas_Object* conformant)
{
	//m_layout
	try
	{
		m_naviframe = naviframe;
		m_conformant = conformant;
		m_box = createbox(naviframe, conformant);

	//	m_layout = createlayout(framewnd, conformant);
		return decorateview(m_box);
	}
	catch(const char* msg)
	{
		//how to delete evas_object ?
		dlog_print(DLOG_FATAL, "View", msg);
		return false;
	}
	return true;
}
void View::DestroyView()
{
	//elm_box_clear(m_box);
	elm_naviframe_item_pop(m_naviframe);
	m_box = NULL;
	m_naviframe = NULL;
	m_conformant = NULL;

	destroyremains();
}

bool View::IsCreated()
{
	return (m_box)?true:false;
}

void View::Show()
{
	if(m_box)
	{
		evas_object_show(m_box);
		showremains();
	}
}

Evas_Object* View::createbox(Evas_Object* naviframe, Evas_Object* conformant)
{
	Evas_Object *box = elm_box_add(conformant);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	m_Naviitem = elm_naviframe_item_push(naviframe, getedcfilename(), NULL, NULL, box, NULL);
	evas_object_show(box);

	return box;
}

Evas_Object* View::createlayout(Evas_Object* framewnd, Evas_Object* conformant)
{
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	Evas_Object* layout = NULL;
	char edj_path[PATH_MAX] = {0, };
	getresource(getedcfilename(), edj_path, (int)PATH_MAX);
	layout = elm_layout_add(framewnd);
	if(layout == NULL)
	{
		throw "fail to create layout";
	}
	elm_layout_file_set(layout, edj_path, GRP_MAIN);
	elm_object_part_text_set(layout, "txt_title", "hello tizen");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(layout, EEXT_CALLBACK_BACK, layout_back_cb, this);
	elm_object_content_set(conformant, layout);

	return layout;
}

void View::handlelayoutevent(Evas_Object *obj, void *event_info)
{
	/* Let window go to hide state. */
//	elm_win_lower(m_box);
}

void View::layout_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	View* view = static_cast<View*>(data);
	if(view != NULL)
	{
		view->handlelayoutevent(obj, event_info);
		return;
	}
	//assert
}

void View::getresource(const char *edj_file_in, char *edj_path_out, int edj_path_max)
{
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(edj_path_out, edj_path_max, "%s%s", res_path, edj_file_in);
		free(res_path);
	}
}

