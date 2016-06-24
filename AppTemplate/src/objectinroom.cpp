/*
 * objectinroom.cpp
 *
 *  Created on: Jun 23, 2016
 *      Author: Jason
 */

#include "objectinroom.h"
#include "apptemplate.h"
#include <string>

ObjectinRoom::ObjectinRoom()
:m_state(NORMAL_STATE), m_inroom(false)
{
	m_iconname[ROOM_OBJECT_LISTENER]="images/listener.png";
	m_iconname[ROOM_OBJECT_SOURCE_1]="images/source_1.png";
	m_iconname[ROOM_OBJECT_SOURCE_2]="images/source_2.png";
	m_iconname[ROOM_OBJECT_SOURCE_3]="images/source_3.png";
	m_iconname[ROOM_OBJECT_SOURCE_4]="images/source_4.png";
	m_iconname[ROOM_OBJECT_SOURCE_5]="images/source_5.png";
}

ObjectinRoom::~ObjectinRoom()
{

}

void ObjectinRoom::Create(Evas_Coord x, Evas_Coord y, RoomObjectType obj_type, Evas_Object* parent)
{
	m_size = (int) 40 * elm_config_scale_get();
	m_object = elm_image_add(evas_object_evas_get(parent));
	std::string resource_path = app_get_resource_path();
	resource_path += m_iconname[obj_type];
	elm_image_file_set(m_object, resource_path.c_str(), NULL);
	if (evas_object_image_load_error_get(m_object) != EVAS_LOAD_ERROR_NONE)
	   return;
	evas_object_resize(m_object, m_size, m_size);
	evas_object_move(m_object, x - (m_size/2), y - (m_size/2));
	evas_object_pass_events_set(m_object, EINA_TRUE);

	m_parent = parent;

	if(obj_type == ROOM_OBJECT_LISTENER)
		Enter();
}

void ObjectinRoom::Destroy()
{
	evas_object_del(m_object);
}

void ObjectinRoom::Hide()
{
	evas_object_hide(m_object);
}

void ObjectinRoom::Show()
{
	if(m_inroom)
		evas_object_show(m_object);
}

void ObjectinRoom::Enter()
{
	m_inroom = true;
	if(evas_object_visible_get(m_parent)==EINA_TRUE)
	{
		Show();
	}
}

void ObjectinRoom::Out()
{
	m_inroom = false;
	Hide();
}

bool ObjectinRoom::IsinRoom()
{
	return m_inroom;
}

void ObjectinRoom::HandleDownEvent(Evas_Coord x, Evas_Coord y)
{
	if(m_inroom && isthispointinrect(x, y))
	{
		m_state = SELECTED_STATE;
	}
}

void ObjectinRoom::HandleUpEvent(Evas_Coord x, Evas_Coord y)
{
	if(m_inroom && m_state == SELECTED_STATE)
	{
		m_state = NORMAL_STATE;
	}
}
void ObjectinRoom::HandleMoveEvent(Evas_Coord x, Evas_Coord y)
{
	if(m_inroom && m_state == SELECTED_STATE)
	{
		if(ismoveavailable(x, y))
		{
			evas_object_move(m_object, x - (m_size/2), y - (m_size/2));
		}
	}
}

EvasCoordRect ObjectinRoom::GetRect()
{
	EvasCoordRect rect;
	evas_object_geometry_get(m_object, &rect.x, &rect.y, &rect.w, &rect.h);
	return rect;
}

bool ObjectinRoom::IsSelectedState()
{
	return (m_state == SELECTED_STATE)? true:false;
}

bool ObjectinRoom::ismoveavailable(Evas_Coord x, Evas_Coord y)
{
	EvasCoordRect canvasrect;
	evas_object_geometry_get(m_parent, &canvasrect.x, &canvasrect.y, &canvasrect.w, &canvasrect.h);
	int half = m_size/2;
	if(x - half >= canvasrect.x && y - half >= canvasrect.y
			&& x+ half < canvasrect.x + canvasrect.w &&  y+ half < canvasrect.y + canvasrect.h )
		return true;
	return false;
}

bool ObjectinRoom::isthispointinrect(Evas_Coord x, Evas_Coord y)
{
	EvasCoordRect rect;
	evas_object_geometry_get(m_object, &rect.x, &rect.y, &rect.w, &rect.h);
	if(x >= rect.x && y >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h)
		return true;
	return false;
}

