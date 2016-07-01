/*
 * popupprogress.h
 *
 *  Created on: Jun 30, 2016
 *      Author: Jason
 */

#ifndef POPUPPROGRESS_H_
#define POPUPPROGRESS_H_

#include "base.h"
#include <Elementary.h>

typedef void (*Porgress_Cancel_Cb)(void *data); //for cancel

class PopupProgress
{
public:
	PopupProgress();
	~PopupProgress();

	void Create(Evas_Object* parent, Porgress_Cancel_Cb cancel_cb, void* data);
	void Destroy();

	void Popup();
	void Hide();

	void SetValue(double value);

private:
	void cancel();
	Evas_Object* createpopup(Evas_Object* parent);
	Evas_Object* createpb(Evas_Object* parent);
	Evas_Object* createcancelbtn(Evas_Object* parent);
	static void clicked_cancel_cb(void *data, Evas_Object *obj, void *event_info);

private:
	Evas_Object* m_cancelbtn;
	Evas_Object* m_pb;
	Evas_Object* m_popup;
	Evas_Object* m_parent;

	Porgress_Cancel_Cb m_cancelcb;
	void* m_data;
};



#endif /* POPUPPROGRESS_H_ */

