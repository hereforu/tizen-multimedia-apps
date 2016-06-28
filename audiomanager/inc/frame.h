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



class FrameWindow
{
public:
	FrameWindow();
	~FrameWindow();

	void CreateBaseFrame();
	void AddView(View* view);


	View* GetCurrentView();
	void MoveNextView();
	void MovePrevView();
	void ActivateFirstView();

	void Show();


private:
	void pushview(View* view);
	void popview(View* view);
	void deleteallviews();
	Evas_Object* createframewindow();
	Evas_Object* createconformation(Evas_Object* parentwnd);
	Evas_Object* createnaviframe(Evas_Object* conformant);
	void handledeleterequest(Evas_Object *obj, void *event_info);

	//for callback
	static void delete_request_cb(void *data, Evas_Object *obj, void *event_info);
	static void on_backkeydown_cb(void *data, Evas_Object *obj, void *event_info);


private:
	Evas_Object* m_framewnd;
	Evas_Object* m_conformant;
	Evas_Object* m_naviframe;
	std::vector<View*> m_views;
	int m_currentviewindex;
};


#endif /* FRAME_H_ */
