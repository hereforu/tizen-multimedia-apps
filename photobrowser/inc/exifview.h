/*
 * exifview.h
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */

#ifndef EXIFVIEW_H_
#define EXIFVIEW_H_


#include "common/view.h"
#include "common/listctrl.h"
#include "common/buttonpack.h"
#include "exif.h"


class ExifView : public View
{
public:
	ExifView();
	~ExifView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();



private:
	std::string getresiconpath(const char* iconname);
	void add_defaultbtns(ButtonPack& btnpack);
	void setinfo_tolist(ListCtrl& list, EXIF& exif);
	void change_optionview(int id);
	void move_prev();
	void play();



	static void clicked_prev_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_play_cb(void *data, Evas_Object *obj, void *event_info);

	static void change_optionview_cb(void *data, int id);


private:


	ListCtrl m_list;
	ButtonPack m_btnpack;

};


#endif /* EXIFVIEW_H_ */
