/*
 * srcSelectionView.h
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 * Description: Select audio sources.
 */

#ifndef SRCSELECTIONVIEW_H_
#define SRCSELECTIONVIEW_H_

#include "view.h"

class SrcSelectionView : public View
{
public:
	SrcSelectionView();
	~SrcSelectionView();

	void initSelectedSrc();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();

	char* strToChar(std::string str);

private:
	static char* genlist_text_get_cb(void *data, Evas_Object *obj, const char *part);
	static Evas_Object* genlist_content_get_cb(void *data, Evas_Object *obj, const char *part);
	Elm_Genlist_Item_Class* createItc();

	//Evas_Object* createIcon(Evas_Object* box, std::string iconPath);

	// list
	static void selected_cb(void *data, Evas_Object *obj, void *event_info);
	void createList(Evas_Object* box);

	// button
	static void backBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);
	void createBackBtn(Evas_Object* box);

	// request data to model
	int getSrcNum();
	const char* getSrcNameByIdx(int idx);

	//Eina_List* getList_selected(); //request to itself?

private:
	int m_srcNum;
	int* m_list_selectedSrcIdx;
	int m_maxSelection;
};



#endif /* SRCSELECTIONVIEW_H_ */
