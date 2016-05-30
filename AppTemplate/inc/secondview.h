/*
 * secondview.h
 *
 *  Created on: May 26, 2016
 *      Author: Jason
 */

#ifndef SECONDVIEW_H_
#define SECONDVIEW_H_


#include "view.h"

class SecondView : public View
{
public:
	SecondView();
	~SecondView();

protected:
	const char* getedcfilename();
	bool decorateview(Evas_Object* box);
	void destroyremains();
	void showremains();
private:
	static void clicked_cb_in_secondview(void *data, Evas_Object *obj, void *event_info);
private:
	Evas_Object* m_label;

};


#endif /* SECONDVIEW_H_ */
