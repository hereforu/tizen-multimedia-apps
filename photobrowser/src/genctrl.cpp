/*
 * genctrl.cpp
 *
 *  Created on: Jun 23, 2016
 *      Author: Jason
 */

#include "genctrl.h"
#include <stdexcept>



GenCtrl::GenCtrl()
	:m_selectedid(0), m_cbtoview(NULL), m_cbtoview_data(NULL), m_parent(NULL), m_ctrl(NULL)
{

}
GenCtrl::~GenCtrl()
{

}
void GenCtrl::Create(Evas_Object* parent, GenCtrl_Select_Cb selectcb_toview, void* data, int iconsize)
{
	m_parent = parent;
	m_cbtoview = selectcb_toview;
	m_cbtoview_data = data;
	m_iconsize = iconsize;

	if((m_ctrl = creategenctrl(parent))==NULL)
	{
		throw std::runtime_error("fail to create list control");
	}

}

void GenCtrl::Destroy()
{
	m_itemmap.clear();
	cleancbdatastore();
	evas_object_del(m_ctrl);
}

Evas_Object* GenCtrl::GetCtrl()
{
	return m_ctrl;
}

void GenCtrl::AppendItem(GenCtrlItem& item)
{
	Elm_Gen_Item_Class* itc = createitc(item_label_get_cb, item_content_get_cb, NULL);
	DataforGenCtrlCB* data = generatecbdata(item);
	item.it = appenditem(itc, data, item_selected_cb);
	m_itemmap[item.id] = item;
	freeitc(itc);
}

void GenCtrl::AppendItems(std::vector<GenCtrlItem>& items)
{
	Elm_Gen_Item_Class* itc = createitc(item_label_get_cb, item_content_get_cb, NULL);
	for(unsigned int i = 0; i < items.size(); ++i)
	{
		DataforGenCtrlCB* data = generatecbdata(items[i]);
		items[i].it = appenditem(itc, data, item_selected_cb);
		m_itemmap[items[i].id] = items[i];
	}
	freeitc(itc);
}

void GenCtrl::SelectItem(int id)
{
	//elm_genlist_item_selected_set (m_itemmap[id].it, EINA_TRUE);
}


void GenCtrl::RemoveAllItems()
{
	removeallitems();
	m_itemmap.clear();
	cleancbdatastore();
}

const GenCtrlItem& GenCtrl::GetItem(int id)
{
	return m_itemmap[id];
}

const GenCtrlItem& GenCtrl::GetSelectedItem()
{
	return m_itemmap[m_selectedid];
}


Evas_Object* GenCtrl::getctrl()
{
	return m_ctrl;
}

DataforGenCtrlCB* GenCtrl::generatecbdata(const GenCtrlItem& item)
{
	DataforGenCtrlCB* data = new DataforGenCtrlCB((void*)this, item.id);
	m_cbdatastore.push_back(data);
	return data;
}

void GenCtrl::cleancbdatastore()
{
	for(int i = 0; i < m_cbdatastore.size(); ++i)
	{
		delete m_cbdatastore[i];
	}
	m_cbdatastore.clear();
}


char* GenCtrl::get_item_label(int id)
{
	if(m_itemmap.find(id) == m_itemmap.end())
	{
		dlog_print(DLOG_DEBUG, "GenCtrl", "invalid item: %d", id);
		return NULL;
	}
	return strdup(m_itemmap[id].displayname.c_str());
}

Evas_Object* GenCtrl::get_item_content(int id, Evas_Object *obj)
{
	if(m_itemmap.find(id) == m_itemmap.end())
	{
		dlog_print(DLOG_DEBUG, "GenCtrl", "invalid item: %d", id);
		return NULL;
	}

	Evas_Object* thumbnail = elm_image_add(obj);
	elm_image_file_set(thumbnail, m_itemmap[id].thumbnail_path.c_str(), NULL);
	evas_object_size_hint_min_set(thumbnail, m_iconsize, m_iconsize);
	evas_object_size_hint_max_set(thumbnail, m_iconsize, m_iconsize);

	if (evas_object_image_load_error_get(thumbnail) != EVAS_LOAD_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "GenCtrl", "fail to load thumb: %s", m_itemmap[id].thumbnail_path.c_str());
		return NULL;
	}

	return thumbnail;
}

void GenCtrl::selected_item(int id)
{
	m_selectedid = id;
	if(m_cbtoview)
	{
		m_cbtoview(m_cbtoview_data, id);
	}
}
char* GenCtrl::item_label_get_cb(void *data, Evas_Object *obj, const char *part)
{
	dlog_print(DLOG_DEBUG, "GenCtrl", "call item_label_get_cb callback");
	if(strcmp(part, "elm.text") == 0)
	{
		DataforGenCtrlCB* ctrlandid = (DataforGenCtrlCB*)data;
		GenCtrl* genctrl = (GenCtrl*)ctrlandid->genctrl;
		return genctrl->get_item_label(ctrlandid->id);
	}
	return NULL;
}

Evas_Object* GenCtrl::item_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	dlog_print(DLOG_DEBUG, "GenCtrl", "call item_content_get_cb callback");
	if (strcmp(part, "elm.swallow.icon") == 0)
	{
		DataforGenCtrlCB* ctrlandid = (DataforGenCtrlCB*)data;
		GenCtrl* genctrl = (GenCtrl*)ctrlandid->genctrl;
		return genctrl->get_item_content(ctrlandid->id, obj);
	}
	return NULL;
}


void GenCtrl::item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	DataforGenCtrlCB* ctrlandid = (DataforGenCtrlCB*)data;

	dlog_print(DLOG_DEBUG, "GenCtrl", "item_selected_cb (%d)", ctrlandid->id);

	GenCtrl* genctrl = (GenCtrl*)ctrlandid->genctrl;
	genctrl->selected_item(ctrlandid->id);

}


