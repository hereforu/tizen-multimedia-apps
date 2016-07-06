/*
 * genctrl.h
 *
 *  Created on: Jun 23, 2016
 *      Author: Jason
 */

#ifndef GENCTRL_H_
#define GENCTRL_H_

#include "base.h"
#include <Elementary.h>
#include <string>
#include <map>
#include <vector>

typedef struct _GenCtrlItem
{
	_GenCtrlItem(int _id, const char* _displayname, const char* _media_path, const char* _thumbnail_path)
	:id(_id), displayname(_displayname),media_path(_media_path), thumbnail_path(_thumbnail_path), it(NULL)
	{}
	_GenCtrlItem()
	:it(NULL)
	{

	}
	int id;
	std::string displayname;
	std::string media_path;
	std::string thumbnail_path;
	Elm_Object_Item* it;
}GenCtrlItem;

typedef struct _DataforGenCtrlCB
{
	_DataforGenCtrlCB(void* _genctrl, int _id)
	:genctrl(_genctrl), id(_id)
	{

	}
	void* genctrl;
	int id;
}DataforGenCtrlCB;

typedef void (*GenCtrl_Select_Cb)(void *data, int index);

class GenCtrl
{
public:
	enum
	{
		BIG_ITEM_SIZE = 100,
		SMALL_ITEM_SIZE = 50
	};

	GenCtrl();
	virtual ~GenCtrl();

	void Create(Evas_Object* parent, GenCtrl_Select_Cb selectcb_toview = NULL, void* data = NULL, int iconsize = BIG_ITEM_SIZE);
	void Destroy();

	void AppendItem(GenCtrlItem& item);
	void AppendItems(std::vector<GenCtrlItem>& items);

	void RemoveAllItems();
	const GenCtrlItem& GetItem(int id);
	const GenCtrlItem& GetSelectedItem();
	void SelectItem(int id);
	Evas_Object* GetCtrl();


protected:
	Evas_Object* getctrl();
	virtual Evas_Object* creategenctrl(Evas_Object* parent) = 0;
	virtual Elm_Object_Item* appenditem(Elm_Gen_Item_Class* itc, DataforGenCtrlCB* cbdata, Evas_Smart_Cb selectcb) = 0;
	virtual Elm_Gen_Item_Class* createitc(Elm_Gen_Item_Text_Get_Cb textcb, Elm_Gen_Item_Content_Get_Cb contentcb, Elm_Gen_Item_Del_Cb delcb) = 0;
	virtual void freeitc(Elm_Gen_Item_Class* itc) = 0;
	virtual void removeallitems() = 0;


private:
	void selected_item(int id);
	virtual char* get_item_label(int id);
	virtual Evas_Object* get_item_content(int id, Evas_Object *obj);
	DataforGenCtrlCB* generatecbdata(const GenCtrlItem& item);
	void cleancbdatastore();
	static char* item_label_get_cb(void *data, Evas_Object *obj, const char *part);
	static Evas_Object* item_content_get_cb(void *data, Evas_Object *obj, const char *part);
	static void item_selected_cb(void *data, Evas_Object *obj, void *event_info);

private:
	int m_iconsize;
	int m_selectedid;
	GenCtrl_Select_Cb m_cbtoview;
	void* m_cbtoview_data;

	Evas_Object* m_parent;
	Evas_Object* m_ctrl;
	std::map<int, GenCtrlItem> m_itemmap;
	std::vector<DataforGenCtrlCB*> m_cbdatastore;
};


#endif /* GENCTRL_H_ */
