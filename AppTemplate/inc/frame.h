/*
 * frame.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <Elementary.h>
#include <vector>
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
	void AddView(View* view);


	View* GetCurrentView();
	bool MoveNextView();
	bool MovePrevView();
	bool ActivateFirstView();

	void Show();


private:
	bool pushview(View* view);
	void popview(View* view);
	void deleteallviews();
	Evas_Object* createframewindow();
	Evas_Object* createconformation(Evas_Object* parentwnd);
	Evas_Object* createnaviframe(Evas_Object* conformant);
	void handledeleterequest(Evas_Object *obj, void *event_info);

	//for callback
	static void delete_request_cb(void *data, Evas_Object *obj, void *event_info);


private:
	Evas_Object* m_framewnd;
	Evas_Object* m_conformant;
	Evas_Object* m_naviframe;
	std::vector<View*> m_views;
	int m_currentviewindex;
};


#endif /* FRAME_H_ */
