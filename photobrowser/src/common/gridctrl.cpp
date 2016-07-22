/*
 * gridctrl.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */

#include "common/gridctrl.h"

GridCtrl::GridCtrl()
{

}
GridCtrl::~GridCtrl()
{

}


Evas_Object* GridCtrl::creategenctrl(Evas_Object* parent)
{
	Evas_Object* ctrl = elm_gengrid_add(parent);
	if(ctrl!=NULL)
	{
		evas_object_size_hint_weight_set(ctrl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(ctrl, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_object_style_set(ctrl, "default");
	//	elm_gengrid_select_mode_set(ctrl, ELM_OBJECT_SELECT_MODE_ALWAYS);
		elm_box_pack_end(parent, ctrl);
		evas_object_show(ctrl);
	}
	return ctrl;
}
Elm_Object_Item* GridCtrl::appenditem(GenCtrlItem& item, Elm_Gen_Item_Class* itc, DataforGenCtrlCB* cbdata, Evas_Smart_Cb selectcb)
{
	return elm_gengrid_item_append(getctrl(), itc, (void*)cbdata, selectcb, (void*)cbdata);
}

Elm_Gen_Item_Class* GridCtrl::createitc(Elm_Gen_Item_Text_Get_Cb textcb, Elm_Gen_Item_Content_Get_Cb contentcb, Elm_Gen_Item_Del_Cb delcb)
{
	Elm_Gengrid_Item_Class *itc = elm_gengrid_item_class_new();
	if(itc != NULL)
	{
		itc->item_style = "default";
		itc->func.text_get = textcb;
		itc->func.content_get = contentcb;
		itc->func.state_get = NULL;
		itc->func.del = delcb;
	}

	return (Elm_Gen_Item_Class*)itc;

}
void GridCtrl::freeitc(Elm_Gen_Item_Class* itc)
{
	elm_gengrid_item_class_free((Elm_Gengrid_Item_Class*)itc);
}

void GridCtrl::removeallitems()
{
	elm_gengrid_clear(getctrl());
}

