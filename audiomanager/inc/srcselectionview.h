/*
 * srcSelectionView.h
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 * Description: Select audio sources.
 */

#ifndef SRCSELECTIONVIEW_H_
#define SRCSELECTIONVIEW_H_

#include "common/view.h"
#include "common/multimediaapp.h"
#include "audioManagerModel.h"
#include <stdexcept>
#include <assert.h>
#include <vector>
#include <string>

typedef std::string String;
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
	AudioManagerModel* getModel();

	// genlist
	static char* genlist_text_get_cb(void *data, Evas_Object *obj, const char *part);
	static Evas_Object* genlist_content_get_cb(void *data, Evas_Object *obj, const char *part);
	Elm_Genlist_Item_Class* createItc();
	Evas_Object* get_genlist_content(int idx, Evas_Object *obj, void *data);

	// checkbox
	static void src_selected_cb(void *data, Evas_Object *obj, void *event_info);
	void select_source(int idx, Evas_Object *obj);
	bool delete_if_alreadyselected(int idx);
	bool isalreadyselected(int idx);


	// button
	static void backBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);
	void createBackBtn(Evas_Object* box);

	// popup
	static void popupTimeout_cb(void *data, Evas_Object *obj, void *event_info);
	void popAlarm(const char* str);

	// request data to model
	void getSelectedSrc();
	const char* getsourcedisplayname(int idx);

private:
	int m_srcNum;
	const int m_maxSelection;

	std::vector<unsigned int> m_selectedidxvec;
	UsrData *m_list_usrData;
};


#endif /* SRCSELECTIONVIEW_H_ */
