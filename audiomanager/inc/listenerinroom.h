/*
 * listenerinroom.h
 *
 *  Created on: Jun 25, 2016
 *      Author: Jason
 */

#ifndef LISTENERINROOM_H_
#define LISTENERINROOM_H_


#include "objectinroom.h"


class ListenerinRoom : public ObjectinRoom
{
public:
	ListenerinRoom();
	~ListenerinRoom();

	void Create(Evas_Coord x, Evas_Coord y, Evas_Object* parent);

protected:
	const char* geticonpath();
};

#endif /* LISTENERINROOM_H_ */
