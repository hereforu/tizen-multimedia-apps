/*
 * optionview.h
 *
 *  Created on: Jun 30, 2016
 *      Author: Jason
 */

#ifndef OPTIONVIEW_H_
#define OPTIONVIEW_H_



#include "common/view.h"
#include "common/listctrl.h"
#include "transcodermodel.h"
#include "common/buttonpack.h"


class OptionView : public View
{
public:
	OptionView();
	~OptionView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();



private:
	Evas_Object* createdisplay(Evas_Object* box);
	std::string getresiconpath(const char* iconname);
	void add_defaultbtns(ButtonPack& btnpack);
	void setinfo_tolist(ListCtrl& list, const MediaContentItem& content);
	void chooseone(int id);

	static void clicked_prev_cb(void *data, Evas_Object *obj, void *event_info);
	static void choose_one_cb(void *data, int index);

private:

	ListCtrl m_list;
	ButtonPack m_btnpack;
};



#endif /* OPTIONVIEW_H_ */
