/*
 * listctrl.h
 *
 *  Created on: Jun 23, 2016
 *      Author: Jason
 */

#ifndef LISTCTRL_H_
#define LISTCTRL_H_


#include <Elementary.h>
#include <string>
#include <map>
#include <vector>


typedef struct _ListItem
{
	int media_id;
	std::string displayname;
	std::string media_path;
	std::string thumbnail_path;
}ListItem;


typedef struct _DataforListCrlCB
{
	_DataforListCrlCB(void* _listctrl, int _media_id)
	:listctrl(_listctrl), media_id(_media_id)
	{

	}
	void* listctrl;
	int media_id;
}DataforListCrlCB;

class ListCtrl
{
public:
	ListCtrl();
	~ListCtrl();

	void Create(Evas_Object* parent);
	void Destroy();

	void AppendItem(const ListItem& item);
	void RemoveAllItems();
	const ListItem& GetItem(int media_id);
	const ListItem& GetSelectedItem();

private:
	void cleancbdatastore();
	Evas_Object* createlist(Evas_Object* parent);
	Elm_Genlist_Item_Class* createitc();
	char* get_item_label(int media_id);
	Evas_Object* get_item_content(int media_id, Evas_Object *obj);

	static char* item_label_get_cb(void *data, Evas_Object *obj, const char *part);
	static Evas_Object* item_content_get_cb(void *data, Evas_Object *obj, const char *part);
	static void item_selected_cb(void *data, Evas_Object *obj, void *event_info);

private:
	Evas_Object* m_parent;
	Evas_Object* m_list;
	Elm_Genlist_Item_Class* m_itc;
	std::map<int, ListItem> m_itemmap;
	std::vector<DataforListCrlCB*> m_cbdatastore;



};


#endif /* LISTCTRL_H_ */
