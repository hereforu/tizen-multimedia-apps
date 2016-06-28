/*
 * listctrl.cpp
 *
 *  Created on: Jun 23, 2016
 *      Author: Jason
 */
#include "base.h"
#include "listctrl.h"
#include <stdexcept>



ListCtrl::ListCtrl()
	:m_parent(NULL), m_list(NULL), m_itc(NULL)
{

}
ListCtrl::~ListCtrl()
{

}
void ListCtrl::Create(Evas_Object* parent)
{
	m_parent = parent;
	if((m_list = createlist(parent))==NULL)
	{
		throw std::runtime_error("fail to create list control");
	}

	if((m_itc = createitc())==NULL)
	{
		throw std::runtime_error("fail to create an itc of list control");
	}
}

void ListCtrl::Destroy()
{
	cleancbdatastore();

	elm_genlist_item_class_free(m_itc);
	evas_object_del(m_list);
}

void ListCtrl::AppendItem(const ListItem& item)
{
	m_itemmap[item.media_id] = item;
	DataforListCrlCB* data = new DataforListCrlCB((void*)this, item.media_id);
	m_cbdatastore.push_back(data);
	Elm_Object_Item* objectitem = elm_genlist_item_append(m_list, m_itc, (void*)data, NULL, ELM_GENLIST_ITEM_NONE, item_selected_cb, (void*)data);
	if(objectitem ==NULL)
	{
		throw std::runtime_error(std::string("fail to append")+item.displayname);
	}
}

void ListCtrl::RemoveAllItems()
{
	elm_genlist_clear(m_list);
	m_itemmap.clear();
	cleancbdatastore();
}

const ListItem& ListCtrl::GetItem(int media_id)
{
	return m_itemmap[media_id];
}

const ListItem& ListCtrl::GetSelectedItem()
{
	return m_itemmap[0];
}

void ListCtrl::cleancbdatastore()
{
	for(int i = 0; i < m_cbdatastore.size(); ++i)
	{
		delete m_cbdatastore[i];
	}
	m_cbdatastore.clear();
}

Evas_Object* ListCtrl::createlist(Evas_Object* parent)
{
	Evas_Object* list = elm_genlist_add(parent);
	if(list!=NULL)
	{
		evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_genlist_homogeneous_set(list, EINA_TRUE); // make all items the same height (no need to calculate)
		elm_genlist_select_mode_set(list, ELM_OBJECT_SELECT_MODE_NONE);
		elm_genlist_mode_set(list, ELM_LIST_SCROLL);
		elm_box_pack_end(m_parent, m_list);
		evas_object_show(m_list);

	}
	return list;
}

Elm_Genlist_Item_Class* ListCtrl::createitc()
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	if(itc != NULL)
	{
		itc->item_style = "default";
		itc->func.text_get = item_label_get_cb;;
		itc->func.content_get = item_content_get_cb;
		itc->func.state_get = NULL;
		itc->func.del = NULL;
	}
	return itc;
}
char* ListCtrl::get_item_label(int media_id)
{
	if(m_itemmap.find(media_id) == m_itemmap.end())
	{
		dlog_print(DLOG_DEBUG, "ListCtrl", "invalid item: %d", media_id);
		return NULL;
	}
	return strdup(m_itemmap[media_id].displayname.c_str()); //I believe that genlist would delete the memory after using it
}

Evas_Object* ListCtrl::get_item_content(int media_id, Evas_Object *obj)
{
	if(m_itemmap.find(media_id) == m_itemmap.end())
	{
		dlog_print(DLOG_DEBUG, "ListCtrl", "invalid item: %d", media_id);
		return NULL;
	}
	Evas_Object* thumbnail = elm_image_add(obj);
	elm_image_file_set(thumbnail, m_itemmap[media_id].thumbnail_path.c_str(), NULL);
	if (evas_object_image_load_error_get(thumbnail) != EVAS_LOAD_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "ListCtrl", "fail to load thumb: %s", m_itemmap[media_id].thumbnail_path.c_str());
		return NULL;
	}
	dlog_print(DLOG_DEBUG, "ListCtrl", "load thumb: %s (%p)", m_itemmap[media_id].thumbnail_path.c_str(), thumbnail);


	return thumbnail;
}
char* ListCtrl::item_label_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if(strcmp(part, "elm.text") == 0)
	{
		DataforListCrlCB* ctrlandid = (DataforListCrlCB*)data;
		ListCtrl* listcrl = (ListCtrl*)ctrlandid->listctrl;
		return listcrl->get_item_label(ctrlandid->media_id);
	}
	return NULL;
}

Evas_Object* ListCtrl::item_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (strcmp(part, "elm.swallow.icon") == 0)
	{
		DataforListCrlCB* ctrlandid = (DataforListCrlCB*)data;
		ListCtrl* listcrl = (ListCtrl*)ctrlandid->listctrl;
		return listcrl->get_item_content(ctrlandid->media_id, obj);
	}
	return NULL;
}


void ListCtrl::item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	DataforListCrlCB* ctrlandid = (DataforListCrlCB*)data;
	ListCtrl* listcrl = (ListCtrl*)ctrlandid->listctrl;
}


