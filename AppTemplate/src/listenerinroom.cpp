/*
 * listenerinroom.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: Jason
 */

#include "listenerinroom.h"


ListenerinRoom::ListenerinRoom()
{

}
ListenerinRoom::~ListenerinRoom()
{

}
void ListenerinRoom::Create(Evas_Coord x, Evas_Coord y, Evas_Object* parent)
{
	create(x, y, parent);
	enter();
}
const char* ListenerinRoom::geticonpath()
{
	return "images/listener.png";
}

