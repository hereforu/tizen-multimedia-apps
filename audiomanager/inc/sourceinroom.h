/*
 * sourceinroom.h
 *
 *  Created on: Jun 25, 2016
 *      Author: Jason
 */

#ifndef SOURCEINROOM_H_
#define SOURCEINROOM_H_


#include "objectinroom.h"


typedef enum  _RoomSourceType
{

	ROOM_SOURCE_0 = 0,
	ROOM_SOURCE_1,
	ROOM_SOURCE_2,
	ROOM_SOURCE_3,
	ROOM_SOURCE_4,
	ROOM_SOURCE_MAX
}RoomSourceType;

typedef struct _RoomSourceProperty
{
	_RoomSourceProperty()
	:sourceidx(-1)
	{

	}
	_RoomSourceProperty(int _idx, const char* _name)
		:sourceidx(_idx), name(_name)
	{

	}
	int sourceidx;
	std::string name;
}RoomSourceProperty;

class SourceinRoom : public ObjectinRoom
{
public:
	SourceinRoom();
	~SourceinRoom();

	void Create(Evas_Coord x, Evas_Coord y, RoomSourceType source_type, Evas_Object* parent);
	void SetProperty(const RoomSourceProperty& property);
	void ResetProperty();
	bool IsSameSource(const RoomSourceProperty& property);
	int GetSourceIndex();
	const char* GetIconPath();
	void Enter();
	void Out();



protected:
	const char* geticonpath();

private:
	RoomSourceType m_sourcetype;
	RoomSourceProperty m_property;
};



#endif /* SOURCEINROOM_H_ */
