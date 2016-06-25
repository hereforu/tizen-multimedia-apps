/*
 * sourceinroom.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: Jason
 */

#include "base.h"
#include "sourceinroom.h"



SourceinRoom::SourceinRoom()
{

}
SourceinRoom::~SourceinRoom()
{

}
void SourceinRoom::Create(Evas_Coord x, Evas_Coord y, RoomSourceType source_type, Evas_Object* parent)
{
	m_sourcetype = source_type;
	create(x, y, parent);
}
void SourceinRoom::SetProperty(const RoomSourceProperty& property)
{
	m_property = property;
}
void SourceinRoom::ResetProperty()
{
	m_property.sourceidx = -1;
	m_property.name.clear();
	out();
}
bool SourceinRoom::IsSameSource(const RoomSourceProperty& property)
{
	return (m_property.sourceidx == property.sourceidx && m_property.name == property.name)?true:false;
}
void SourceinRoom::Enter()
{
	AppTool::Assert(m_property.sourceidx != -1);
	enter();
}
int SourceinRoom::GetSourceIndex()
{
	return m_property.sourceidx;
}
void SourceinRoom::Out()
{
	out();
}
const char* SourceinRoom::GetIconPath()
{
	return geticonpath();
}
const char* SourceinRoom::geticonpath()
{
	switch(m_sourcetype)
	{
		case ROOM_SOURCE_0: return "images/source_0.png";
		case ROOM_SOURCE_1: return "images/source_1.png";
		case ROOM_SOURCE_2: return "images/source_2.png";
		case ROOM_SOURCE_3: return "images/source_3.png";
		case ROOM_SOURCE_4: return "images/source_4.png";
		case ROOM_SOURCE_MAX: return "images/default.png";
	}
}
