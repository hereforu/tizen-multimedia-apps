/*
 * buttonpack.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */
#include "apptemplate.h"
#include "base.h"
#include "buttonpack.h"

ButtonPack::ButtonPack()
{

}

ButtonPack::~ButtonPack()
{

}
void ButtonPack::AddPacksHorizontally(Evas_Object* parent, std::vector<BTPackParam>& params)
{
	BTPack pack;

	pack.box = elm_box_add(parent);
	elm_box_horizontal_set(pack.box, EINA_TRUE);
	elm_box_homogeneous_set(pack.box, EINA_TRUE);
	evas_object_size_hint_weight_set(pack.box, 0.0, 0.0);
	evas_object_size_hint_align_set(pack.box,  EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(pack.box);
	elm_box_pack_end(parent, pack.box);

	unsigned int num_btns = params.size();
	for(unsigned int i = 0; i < num_btns; ++i)
	{
		Evas_Object* btn = elm_button_add(parent);
		if(params[i].image_path.empty()==false)
		{
			std::string resource_path = app_get_resource_path();
			resource_path += params[i].image_path;
			Evas_Object *ic;
			ic = elm_icon_add(btn);
			elm_image_file_set(ic, resource_path.c_str(), NULL);
			elm_object_part_content_set(btn, "icon", ic);
			elm_object_style_set(btn,"circle");
		}
		else
		{
			elm_object_text_set(btn, params[i].text.c_str());
		}
		evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_smart_callback_add(btn, "clicked", params[i].cbfunc, params[i].data);
		evas_object_show(btn);
		elm_box_pack_end(pack.box,btn);
		pack.btns.push_back(btn);
	}

	m_btpacks.push_back(pack);
}

void ButtonPack::DestroyAll()
{
	for(unsigned int packidx = 0; packidx < m_btpacks.size(); ++packidx)
	{
		for(unsigned int btnidx = 0; btnidx < m_btpacks[packidx].btns.size(); ++btnidx)
		{
			//TODO:: delete all!!
		}
	}
}


