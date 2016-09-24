/*
 * buttonpack.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "common/buttonpack.h"
#include <stdexcept>

ButtonPack::ButtonPack()
:m_parent(NULL)
{

}

ButtonPack::~ButtonPack()
{

}
void ButtonPack::Create(Evas_Object* parent)
{
	m_parent = parent;
}

void ButtonPack::Destroy()
{
	ResetAllButtons();
	m_parent = NULL;
}

void ButtonPack::AddPacksHorizontally(std::vector<BTPackParam>& params)
{
	BTPack pack;

	pack.box = elm_box_add(m_parent);
	if(pack.box == NULL)
		throw std::runtime_error("fail to create elm_box_add for button pack");
	elm_box_horizontal_set(pack.box, EINA_TRUE);
	elm_box_homogeneous_set(pack.box, EINA_TRUE);
	evas_object_size_hint_weight_set(pack.box, 0.0, 0.0);
	evas_object_size_hint_align_set(pack.box,  EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(pack.box);
	elm_box_pack_end(m_parent, pack.box);

	unsigned int num_btns = params.size();
	for(unsigned int i = 0; i < num_btns; ++i)
	{
		Evas_Object* btn = createbutton(m_parent, params[i]);
		if(btn != NULL)
		{
			evas_object_show(btn);
			elm_box_pack_end(pack.box,btn);
			pack.btns.push_back(btn);
		}
	}
	m_btpacks.push_back(pack);
}

Evas_Object* ButtonPack::createbutton(Evas_Object* parent, const BTPackParam& param)
{
	Evas_Object* btn = elm_button_add(parent);
	if(btn == NULL)
	{
		dlog_print(DLOG_ERROR, "ButtonPack", "fail to create a button in a button pack");
		return NULL;
	}
	if(param.image_path.empty()==false)
	{
		std::string resource_path = app_get_resource_path();
		resource_path += param.image_path;
		Evas_Object *ic;
		ic = elm_icon_add(btn);
		elm_image_file_set(ic, resource_path.c_str(), NULL);
		elm_object_part_content_set(btn, "icon", ic);
		elm_object_style_set(btn,"circle");
	}
	if(param.text.empty()==false)
	{
		elm_object_text_set(btn, param.text.c_str());
	}
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", param.cbfunc, param.data);
	return btn;
}
void ButtonPack::ResetAllButtons()
{
	for(unsigned int packidx = 0; packidx < m_btpacks.size(); ++packidx)
	{
		for(unsigned int btnidx = 0; btnidx < m_btpacks[packidx].btns.size(); ++btnidx)
		{
			SAFE_EVAS_DELETE(m_btpacks[packidx].btns[btnidx]);
		}
		m_btpacks[packidx].btns.clear();
		SAFE_EVAS_DELETE(m_btpacks[packidx].box);
	}
	m_btpacks.clear();
}


