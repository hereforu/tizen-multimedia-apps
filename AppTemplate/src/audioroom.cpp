/*
 * audioroom.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */
#include "apptemplate.h"
#include "base.h"
#include "audioroom.h"
#include <stdexcept>

AudioRoom::AudioRoom()
:m_eventrect(NULL)
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_object[i] = NULL;
	}
}

AudioRoom::~AudioRoom()
{

}

void AudioRoom::Create(Evas_Object* box)
{
	m_eventrect = evas_object_rectangle_add(evas_object_evas_get(box));
	if(m_eventrect == NULL)
	{
		throw std::runtime_error("fail to create an audio room");
	}
	evas_object_size_hint_weight_set(m_eventrect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(m_eventrect, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_color_set(m_eventrect, 255, 255, 255, 255);
	evas_object_repeat_events_set(m_eventrect, EINA_TRUE);
	elm_box_pack_end(box, m_eventrect);
	evas_object_show(m_eventrect);

	evas_object_event_callback_add(box, EVAS_CALLBACK_MOUSE_DOWN, mousedown_cb, this);
	evas_object_event_callback_add(box, EVAS_CALLBACK_MOUSE_MOVE, mousemove_cb, this);
	evas_object_event_callback_add(box, EVAS_CALLBACK_MOUSE_UP, mouseup_cb, this);




	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_object[i] = new ObjectinRoom();
		m_object[i]->Create(100, 200+(i*80),(RoomObjectType)i, m_eventrect);
	}
}

void AudioRoom::HideObjects()
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_object[i]->Hide();
	}
}

void AudioRoom::ShowObjectsIfinRoom()
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_object[i]->Show();
	}
}

void AudioRoom::ToggleSource(RoomObjectType type)
{
	AppTool::Assert(type != ROOM_OBJECT_LISTENER);
	if(m_object[type]->IsinRoom())
		m_object[type]->Out();
	else
		m_object[type]->Enter();
}

void AudioRoom::Destroy()
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_object[i]->Destroy();
		delete m_object[i];
		m_object[i] = NULL;
	}
	evas_object_del(m_eventrect);
	m_eventrect = NULL;
}


void AudioRoom::handledownevent(Evas_Coord x, Evas_Coord y)
{
	for(int i = ROOM_OBJECT_MAX-1; i >= 0; --i)
	{
		m_object[i]->HandleDownEvent(x, y);
		if(m_object[i]->IsSelectedState())
			break;
	}
}
void AudioRoom::handleupevent(Evas_Coord x, Evas_Coord y)
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_object[i]->HandleUpEvent(x, y);
	}
}
void AudioRoom::handlemoveevent(Evas_Coord x, Evas_Coord y)
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_object[i]->HandleMoveEvent(x, y);
	}
}

void AudioRoom::mousedown_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *) event_info;
	Evas_Coord x = ev->output.x;
	Evas_Coord y = ev->output.y;

	AudioRoom* room = (AudioRoom*)data;
	room->handledownevent(x, y);
}

void AudioRoom::mousemove_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Move *ev =(Evas_Event_Mouse_Move *)event_info;
	Evas_Coord x = ev->cur.output.x;
	Evas_Coord y = ev->cur.output.y;
	AudioRoom* room = (AudioRoom*)data;
	room->handlemoveevent(x, y);

}
void AudioRoom::mouseup_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;
	Evas_Coord x = ev->output.x;
	Evas_Coord y = ev->output.y;
	AudioRoom* room = (AudioRoom*)data;
	room->handleupevent(x, y);
}






