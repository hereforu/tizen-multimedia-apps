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


typedef struct _EvasCoordRect
{
	Evas_Coord x;
	Evas_Coord y;
	Evas_Coord w;
	Evas_Coord h;
}EvasCoordRect;

typedef struct _Pos
{
	int x;
	int y;
	int z;
}Pos;

class ObjectinRoom
{
public:
	enum
	{
		NORMAL_STATE = 0,
		SELECTED_STATE
	};
	ObjectinRoom();
	virtual ~ObjectinRoom();


	void Destroy();

	void Hide();
	void Show();

	bool IsinRoom();
	EvasCoordRect GetRect();
	Pos Get3DPos();

	bool IsSelectedState();

	void HandleDownEvent(Evas_Coord x, Evas_Coord y);
	void HandleUpEvent(Evas_Coord x, Evas_Coord y);
	void HandleMoveEvent(Evas_Coord x, Evas_Coord y);

protected:
	void create(Evas_Coord x, Evas_Coord y, Evas_Object* parent);
	virtual const char* geticonpath() = 0;
	void enter();
	void out();

private:
	bool ismoveavailable(Evas_Coord x, Evas_Coord y);
	bool isthispointinrect(Evas_Coord x, Evas_Coord y);

private:
	Evas_Object* m_object;
	Evas_Object* m_parent;
	int m_state;
	int m_size;
	bool m_inroom;
};

#endif /* OBJECTINROOM_H_ */

