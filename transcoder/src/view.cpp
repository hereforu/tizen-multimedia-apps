/*
 * view.cpp
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "base.h"
#include "multimediaapp.h"
#include "view.h"
#include <stdexcept>

View::View()
	:m_box(NULL),m_naviframe(NULL), m_conformant(NULL), m_Naviitem(NULL)
{

}
View::~View()
{
	//how to delete layout??
}

void View::CreateView(Evas_Object* naviframe, Evas_Object* conformant)
{
	try
	{
		m_naviframe = naviframe;
		m_conformant = conformant;
		m_box = createbox(naviframe, conformant);
		decorateview(m_box);
	}
	catch(const std::runtime_error& e)
	{
		//how to delete evas_object ?
		std::string msg = "fail to create view becuase ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void View::DestroyView()
{
	elm_box_clear(m_box);
	elm_naviframe_item_pop(m_naviframe);
	m_box = NULL;
	m_naviframe = NULL;
	m_conformant = NULL;

	destroyremains();
}

bool View::IsCreated()
{
	return (m_box)?true:false;
}



Evas_Object* View::createbox(Evas_Object* naviframe, Evas_Object* conformant)
{
	Evas_Object *box = elm_box_add(conformant);
	if(box == NULL)
	{
		throw std::runtime_error("fail to create a box in a view");
	}
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	m_Naviitem = elm_naviframe_item_push(naviframe, getedcfilename(), NULL, NULL, box, NULL);
	evas_object_show(box);

	return box;
}
TranscoderModel* View::getmodel()
{
	TranscoderModel* model = static_cast<TranscoderModel*>(MODEL);
	AppTool::Assert(model != NULL);
	return model;
}
