/*
 * imagelistview.h
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */

#ifndef IMAGELISTVIEW_H_
#define IMAGELISTVIEW_H_

#include "view.h"
#include "gridctrl.h"
#include "listctrl.h"
#include "photobrowsermodel.h"
#include "buttonpack.h"
#include "exif.h"

class ImageListView : public View
{
public:
	ImageListView();
	~ImageListView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();



private:
	void PrintExif(const char* imagefilename);
	void gotonextview(int id);
	void add_defaultbtns(ButtonPack& btnpack);
	void buildthelist(ListCtrl& list);
	Evas_Object* createpopup(Evas_Object* box);
	void showpopup(const char* message, int timeout=3);
	static void popuptimeout_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_exit_cb(void *data, Evas_Object *obj, void *event_info);
	static void select_image_cb(void *data, int id);


private:
	ListCtrl m_list;
	Evas_Object* m_popup;
	ButtonPack m_btnpack;
};



#endif /* IMAGELISTVIEW_H_ */
