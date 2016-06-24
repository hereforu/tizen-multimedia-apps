/*
 * objectinroom.h
 *
 *  Created on: Jun 23, 2016
 *      Author: Jason
 */

#ifndef OBJECTINROOM_H_
#define OBJECTINROOM_H_

#include <Elementary.h>
#include <string>

typedef enum  _RoomObjectType
{
	ROOM_OBJECT_LISTENER = 0,
	ROOM_OBJECT_SOURCE_1,
	ROOM_OBJECT_SOURCE_2,
	ROOM_OBJECT_SOURCE_3,
	ROOM_OBJECT_SOURCE_4,
	ROOM_OBJECT_SOURCE_5,
	ROOM_OBJECT_MAX
}RoomObjectType;

typedef struct _EvasCoordRect
{
	Evas_Coord x;
	Evas_Coord y;
	Evas_Coord w;
	Evas_Coord h;
}EvasCoordRect;

class ObjectinRoom
{
public:
	enum
	{
		NORMAL_STATE = 0,
		SELECTED_STATE
	};
	ObjectinRoom();
	~ObjectinRoom();

	void Create(Evas_Coord x, Evas_Coord y, RoomObjectType obj_type, Evas_Object* parent);
	void Destroy();

	void Hide();
	void Show();

	void Enter();
	void Out();

	bool IsinRoom();
	EvasCoordRect GetRect();

	bool IsSelectedState();

	void HandleDownEvent(Evas_Coord x, Evas_Coord y);
	void HandleUpEvent(Evas_Coord x, Evas_Coord y);
	void HandleMoveEvent(Evas_Coord x, Evas_Coord y);

private:
	bool ismoveavailable(Evas_Coord x, Evas_Coord y);
	bool isthispointinrect(Evas_Coord x, Evas_Coord y);

private:
	std::string m_iconname[ROOM_OBJECT_MAX];
	Evas_Object* m_object;
	Evas_Object* m_parent;
	int m_state;
	int m_size;
	bool m_inroom;

};


#endif /* OBJECTINROOM_H_ */

