/*
 * audioroom.h
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#ifndef AUDIOROOM_H_
#define AUDIOROOM_H_

#include <Elementary.h>
#include "objectinroom.h"



class AudioRoom
{
public:
	AudioRoom();
	~AudioRoom();

	void Create(Evas_Object* box);
	void Destroy();

	void ToggleSource(RoomObjectType type);
	void HideObjects();
	void ShowObjectsIfinRoom();


private:
	void handledownevent(Evas_Coord x, Evas_Coord y);
	void handleupevent(Evas_Coord x, Evas_Coord y);
	void handlemoveevent(Evas_Coord x, Evas_Coord y);



	static void mousedown_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static void mousemove_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static void mouseup_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);



private:
	ObjectinRoom* m_object[ROOM_OBJECT_MAX];
	Evas_Object* m_eventrect;
};


#endif /* AUDIOROOM_H_ */
