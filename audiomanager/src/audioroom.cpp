/*
 * audioroom.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#include "base.h"
#include "audioroom.h"
#include <stdexcept>

AudioRoom::AudioRoom()
:m_eventrect(NULL)
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i] = NULL;
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

	//TODO: remove all constants
	for(int i = 0; i < ROOM_SOURCE_MAX; ++i)
	{
		SourceinRoom* source = new SourceinRoom();
		source->Create(150 + 500*(i%2), 350+(i*100),(RoomSourceType)i, m_eventrect);
		m_objects[i] = (ObjectinRoom*)source;
	}
	ListenerinRoom* listener = new ListenerinRoom();
	listener->Create(300, 450, m_eventrect);
	m_objects[LISTENER_INDEX] = (ObjectinRoom*)listener;
}

void AudioRoom::ConfigureSources(const std::vector<RoomSourceProperty>& sources)
{
	unsigned int num_sources = sources.size();

	std::vector<RoomSourceProperty> remaining_srcs;
	std::vector<SourceinRoom*> avail_roominsrcs;
	for(int i= 0; i < ROOM_SOURCE_MAX; ++i)
		avail_roominsrcs.push_back(dynamic_cast<SourceinRoom*>(m_objects[i]));


	for(int srcidx = 0; srcidx < num_sources; ++srcidx)
	{
		bool isassigned = false;
		for(std::vector<SourceinRoom*>::iterator iter = avail_roominsrcs.begin(); iter != avail_roominsrcs.end(); ++iter)
		{
			SourceinRoom* srcinroom =*iter;
			if(srcinroom->IsSameSource(sources[srcidx]))
			{
				isassigned = true;
				avail_roominsrcs.erase(iter);
				break;
			}
		}
		if(isassigned == false)
			remaining_srcs.push_back(sources[srcidx]);
	}

	int remaining_srcidx = remaining_srcs.size()-1;
	for(std::vector<SourceinRoom*>::iterator iter = avail_roominsrcs.begin(); iter != avail_roominsrcs.end(); ++iter)
	{
		SourceinRoom* srcinroom =*iter;
		if(remaining_srcidx < 0)
		{
			srcinroom->ResetProperty();
		}
		else
		{
			srcinroom->SetProperty(remaining_srcs[remaining_srcidx]);
			--remaining_srcidx;
		}
	}
}

void AudioRoom::HideObjects()
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i]->Hide();
	}
}

void AudioRoom::ShowObjectsIfinRoom()
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i]->Show();
	}
}

void AudioRoom::ToggleSource(int index)
{
	SourceinRoom* source = findsourcebyindex(index);
	if(source)
	{
		if(source->IsinRoom())
			source->Out();
		else
			source->Enter();
	}
}

Pos AudioRoom::GetListenerPos()
{
	return m_objects[LISTENER_INDEX]->Get3DPos();
}

void AudioRoom::GetPosofSourcesinRoom(std::vector<ObjectPos>& positions)
{
	positions.clear();
	for(int i = 0; i < ROOM_SOURCE_MAX; ++i)
	{
		SourceinRoom* source = dynamic_cast<SourceinRoom*>(m_objects[i]);
		if(source->IsinRoom())
		{
			positions.push_back(ObjectPos(source->GetSourceIndex(),
					source->Get3DPos()));
		}
	}
}
const char* AudioRoom::GetSourceIconPath(int index)
{
	SourceinRoom* source = findsourcebyindex(index);
	if(source)
		return source->GetIconPath();
	return "images/default.png";
}

void AudioRoom::Destroy()
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i]->Destroy();
		delete m_objects[i];
		m_objects[i] = NULL;
	}
	evas_object_del(m_eventrect);
	m_eventrect = NULL;
}
EvasCoordRect AudioRoom::GetRect()
{
	EvasCoordRect rect;
	evas_object_geometry_get(m_eventrect, &rect.x, &rect.y, &rect.w, &rect.h);
	return rect;
}
SourceinRoom* AudioRoom::findsourcebyindex(int index)
{
	for(int i= 0; i < ROOM_SOURCE_MAX; ++i)
	{
		SourceinRoom* source = dynamic_cast<SourceinRoom*>(m_objects[i]);
		if(source->GetSourceIndex() == index)
		{
			return source;
		}
	}
	return NULL;
}
void AudioRoom::handledownevent(Evas_Coord x, Evas_Coord y)
{
	for(int i = ROOM_OBJECT_MAX-1; i >= 0; --i)
	{
		m_objects[i]->HandleDownEvent(x, y);
		if(m_objects[i]->IsSelectedState())
			break;
	}
}
void AudioRoom::handleupevent(Evas_Coord x, Evas_Coord y)
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i]->HandleUpEvent(x, y);
	}
}
void AudioRoom::handlemoveevent(Evas_Coord x, Evas_Coord y)
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i]->HandleMoveEvent(x, y);
	}
}

void AudioRoom::mousedown_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *) event_info;
	Evas_Coord x = ev->output.x;
	Evas_Coord y = ev->output.y;
	dlog_print(DLOG_DEBUG, "AudioRoom", "mouse down: (%d, %d)", x, y);
	AudioRoom* room = (AudioRoom*)data;
	room->handledownevent(x, y);
}

void AudioRoom::mousemove_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Move *ev =(Evas_Event_Mouse_Move *)event_info;
	Evas_Coord x = ev->cur.output.x;
	Evas_Coord y = ev->cur.output.y;
	dlog_print(DLOG_DEBUG, "AudioRoom", "mouse move: (%d, %d)", x, y);
	AudioRoom* room = (AudioRoom*)data;
	room->handlemoveevent(x, y);

}
void AudioRoom::mouseup_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;
	Evas_Coord x = ev->output.x;
	Evas_Coord y = ev->output.y;
	dlog_print(DLOG_DEBUG, "AudioRoom", "mouse up: (%d, %d)", x, y);
	AudioRoom* room = (AudioRoom*)data;
	room->handleupevent(x, y);
}






