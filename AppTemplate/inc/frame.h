/*
 * frame.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <Elementary.h>
#include <map>
#include "view.h"


template<typename TView>
class ViewFactory
{
public:
	static TView* CreateInstance()
	{
		TView* pview = new TView;
		return pview;
	}
};

class FrameWindow
{
public:
	FrameWindow();
	~FrameWindow();

	bool CreateBaseFrame();

	//it returns the view id
	bool AddView(int viewid, View* view);
	View* GetView(int viewid);
	void Show();


private:
	Evas_Object* createframewindow();
	Evas_Object* createconformation(Evas_Object* parentwnd);
	void handledeleterequest(Evas_Object *obj, void *event_info);

	//for callback
	static void delete_request_cb(void *data, Evas_Object *obj, void *event_info);
	View* findview(int viewid);


private:
	Evas_Object* m_framewnd;
	Evas_Object* m_conformant;
	std::map<int, View*> m_viewmap;


};


#endif /* FRAME_H_ */
