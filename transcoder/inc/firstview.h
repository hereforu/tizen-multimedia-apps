/*
 * firstview.h
 *
 *  Created on: May 26, 2016
 *      Author: Jason
 */

#ifndef FIRSTVIEW_H_
#define FIRSTVIEW_H_

#include "view.h"
#include "listctrl.h"

class FirstView : public View
{
public:
	FirstView();
	~FirstView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();



private:
	static void clicked_cb(void *data, Evas_Object *obj, void *event_info);

private:

	Evas_Object* m_btn;
	ListCtrl m_list;

};



#endif /* FIRSTVIEW_H_ */
