/*
 * listctrl.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */



#include "common/listctrl.h"
#include <stdexcept>

ListCtrl::ListCtrl()
{

}
ListCtrl::~ListCtrl()
{

}


Evas_Object* ListCtrl::creategenctrl(Evas_Object* parent)
{
	Evas_Object* ctrl = elm_genlist_add(parent);
	if(ctrl!=NULL)
	{
		evas_object_size_hint_weight_set(ctrl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(ctrl, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_genlist_homogeneous_set(ctrl, EINA_TRUE); // make all items the same height (no need to calculate)
		elm_genlist_multi_select_set (ctrl, EINA_FALSE);
		elm_genlist_select_mode_set(ctrl, ELM_OBJECT_SELECT_MODE_ALWAYS);
		elm_genlist_mode_set(ctrl, ELM_LIST_SCROLL);
		elm_box_pack_end(parent,ctrl);
		evas_object_show(ctrl);
	}
	return ctrl;
}
Elm_Object_Item* ListCtrl::appenditem(GenCtrlItem& item, Elm_Gen_Item_Class* itc, DataforGenCtrlCB* cbdata, Evas_Smart_Cb selectcb)
{
	Elm_Object_Item* objectitem = elm_genlist_item_append(getctrl(), itc, (void*)cbdata, NULL, ELM_GENLIST_ITEM_NONE, selectcb, (void*)cbdata);
	if(objectitem ==NULL)
	{
		dlog_print(DLOG_DEBUG, "GenCtrl", "fail to append an item to list ctrl");
		throw std::runtime_error("fail to append");
	}
	if(item.displayname.size() > 25) //too long to display the given text in a line!
		elm_genlist_item_tooltip_text_set(objectitem, item.displayname.c_str());
	return objectitem;
}

Elm_Gen_Item_Class* ListCtrl::createitc(Elm_Gen_Item_Text_Get_Cb textcb, Elm_Gen_Item_Content_Get_Cb contentcb, Elm_Gen_Item_Del_Cb delcb)
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	if(itc != NULL)
	{
		itc->item_style = "1text.1icon";
		itc->func.text_get = textcb;
		itc->func.content_get = contentcb;
		itc->func.state_get = NULL;
		itc->func.del = delcb;
	}

	return (Elm_Gen_Item_Class*)itc;

}
void ListCtrl::freeitc(Elm_Gen_Item_Class* itc)
{
	elm_genlist_item_class_free((Elm_Gengrid_Item_Class*)itc);
}

void ListCtrl::removeallitems()
{
	elm_genlist_clear(getctrl());
}
