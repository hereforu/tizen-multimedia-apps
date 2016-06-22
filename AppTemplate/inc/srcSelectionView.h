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
#include "multimediaapp.h"
#include "audioManagerModel.h"
#include <stdexcept>
#include <assert.h>
#include <vector>
#include <string>

typedef std::string String;
typedef std::vector<std::string> StrVec;
typedef struct
{
	View* ssv;
	int idx;

} UsrData;


class SrcSelectionView : public View
{
public:
	SrcSelectionView();
	~SrcSelectionView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();

private:
	void createList(Evas_Object* box);

	// genlist
	static char* genlist_text_get_cb(void *data, Evas_Object *obj, const char *part);
	static Evas_Object* genlist_content_get_cb(void *data, Evas_Object *obj, const char *part);
	Elm_Genlist_Item_Class* createItc();

	// checkbox
	static void src_selected_cb(void *data, Evas_Object *obj, void *event_info);

	// button
	static void backBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);
	void createBackBtn(Evas_Object* box);

	// popup
	static void popupTimeout_cb(void *data, Evas_Object *obj, void *event_info);
	void popAlarm(const char* str);

	// request data to model
	StrVec getSrcNameList();
	void getSelectedSrc();

private:
	int m_srcNum;
	static int m_selectedNum;
	static int m_maxSelection;

	StrVec m_list_srcName;
	Eina_Bool *m_list_selectedSrc;

	UsrData *m_list_usrData;
};


#endif /* SRCSELECTIONVIEW_H_ */
