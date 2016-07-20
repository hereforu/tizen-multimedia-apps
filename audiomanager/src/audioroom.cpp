/*
 * audioroom.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "audioroom.h"
#include <stdexcept>

AudioRoom::AudioRoom()
:m_eventrect(NULL), m_gesturelayer(NULL)
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
	try
	{
		m_eventrect = create_eventrect(box);
		m_gesturelayer = create_gesturelayer(box);
		elm_gesture_layer_attach(m_gesturelayer, m_eventrect);
		evas_object_event_callback_add(box, EVAS_CALLBACK_MOUSE_DOWN, mousedown_cb, this);
		evas_object_event_callback_add(box, EVAS_CALLBACK_MOUSE_MOVE, mousemove_cb, this);
		evas_object_event_callback_add(box, EVAS_CALLBACK_MOUSE_UP, mouseup_cb, this);
		create_sources_and_listener(m_eventrect);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "AudioRoom", "Fail to create audio room");
		throw e;
	}
}

Evas_Object* AudioRoom::create_eventrect(Evas_Object* parent)
{
	Evas_Object* eventrect = evas_object_rectangle_add(evas_object_evas_get(parent));
	if(eventrect == NULL)
	{
		throw std::runtime_error("fail to evas_object_rectangle_add in audio room");
	}
	evas_object_size_hint_weight_set(eventrect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(eventrect, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_color_set(eventrect, 255, 255, 255, 255);
	evas_object_repeat_events_set(eventrect, EINA_TRUE);
	elm_box_pack_end(parent, eventrect);
	evas_object_show(eventrect);
	return eventrect;
}

Evas_Object* AudioRoom::create_gesturelayer(Evas_Object* parent)
{
	Evas_Object* gesturelayer = elm_gesture_layer_add(parent);
	if(gesturelayer == NULL)
	{
		throw std::runtime_error("fail to elm_gesture_layer_add in audio room");
	}
	elm_gesture_layer_cb_set(gesturelayer, ELM_GESTURE_N_DOUBLE_TAPS, ELM_GESTURE_STATE_END,
				dbclick_cb, this);
	return gesturelayer;
}

void AudioRoom::create_sources_and_listener(Evas_Object* parent)
{
	for(int i = 0; i < ROOM_SOURCE_MAX; ++i)
	{
		SourceinRoom* source = new SourceinRoom();
		source->Create(150 + 500*(i%2), 350+(i*100),(RoomSourceType)i, parent);
		m_objects[i] = (ObjectinRoom*)source;
	}
	ListenerinRoom* listener = new ListenerinRoom();
	listener->Create(300, 450, parent);
	m_objects[LISTENER_INDEX] = (ObjectinRoom*)listener;
}

void AudioRoom::Destroy()
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i]->Destroy();
		delete m_objects[i];
		m_objects[i] = NULL;
	}
	evas_object_del(m_gesturelayer);
	m_gesturelayer = NULL;
	evas_object_del(m_eventrect);
	m_eventrect = NULL;
}

void AudioRoom::remove_existingsrcs_and_get_newsrcs(const std::vector<RoomSourceProperty>& sources, std::vector<SourceinRoom*>& available_slots, std::vector<RoomSourceProperty>& new_added_srcs)
{
	unsigned int num_sources = sources.size();
	for(int srcidx = 0; srcidx < num_sources; ++srcidx)
	{
		bool isassigned = false;
		for(std::vector<SourceinRoom*>::iterator iter = available_slots.begin(); iter != available_slots.end(); ++iter)
		{
			SourceinRoom* srcinroom =*iter;
			if(srcinroom->IsSameSource(sources[srcidx]))
			{
				isassigned = true;
				available_slots.erase(iter);
				break;
			}
		}
		if(isassigned == false)
		{
			new_added_srcs.push_back(sources[srcidx]);
		}
	}
}

void AudioRoom::locate_new_added_srcs_in_emtpyslots(std::vector<SourceinRoom*>& emptyslots, std::vector<RoomSourceProperty>& new_added_srcs)
{
	int remaining_srcidx = new_added_srcs.size()-1;
	for(std::vector<SourceinRoom*>::iterator iter = emptyslots.begin(); iter != emptyslots.end(); ++iter)
	{
		SourceinRoom* srcinroom =*iter;
		if(remaining_srcidx < 0) //no more sources, let it be empty slot!
		{
			srcinroom->ResetProperty();
		}
		else
		{
			srcinroom->SetProperty(new_added_srcs[remaining_srcidx]);
			--remaining_srcidx;
		}
	}
}

void AudioRoom::ConfigureSources(const std::vector<RoomSourceProperty>& sources)
{
	std::vector<RoomSourceProperty> new_added_srcs;
	std::vector<SourceinRoom*> available_slots;
	for(int i= 0; i < ROOM_SOURCE_MAX; ++i)
	{
		available_slots.push_back(dynamic_cast<SourceinRoom*>(m_objects[i]));
	}

	remove_existingsrcs_and_get_newsrcs(sources, available_slots, new_added_srcs);
	locate_new_added_srcs_in_emtpyslots(available_slots, new_added_srcs);
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

void AudioRoom::handledbclick(Evas_Coord x, Evas_Coord y)
{
	for(int i = 0; i < ROOM_OBJECT_MAX; ++i)
	{
		m_objects[i]->HandleDBClickEvent(x, y);
	}
}

void AudioRoom::mousedown_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *) event_info;
	AudioRoom* room = (AudioRoom*)data;
	room->handledownevent(ev->output.x, ev->output.y);
}

void AudioRoom::mousemove_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Move *ev =(Evas_Event_Mouse_Move *)event_info;
	AudioRoom* room = (AudioRoom*)data;
	room->handlemoveevent(ev->cur.output.x, ev->cur.output.y);

}
void AudioRoom::mouseup_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;
	AudioRoom* room = (AudioRoom*)data;
	room->handleupevent(ev->output.x, ev->output.y);
}

Evas_Event_Flags AudioRoom::dbclick_cb(void *data, void *event_info)
{
	Elm_Gesture_Taps_Info *p = (Elm_Gesture_Taps_Info *) event_info;
	AudioRoom* room = (AudioRoom*)data;
	room->handledbclick(p->x, p->y);
	return EVAS_EVENT_FLAG_ON_HOLD;
}






