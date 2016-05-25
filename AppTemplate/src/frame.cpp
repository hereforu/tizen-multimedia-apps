/*
 * frame.cpp
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "base.h"
#include "apptemplate.h"
#include "frame.h"


FrameWindow::FrameWindow()
:m_framewnd(NULL), m_conformant(NULL)
{

}
FrameWindow::~FrameWindow()
{
	//delete frame window & conformant
	//delete all views
}

bool FrameWindow::CreateBaseFrame()
{
	try
	{
		m_framewnd = createframewindow();
		m_conformant = createconformation(m_framewnd);
	}
	catch(const char* msg)
	{
		//how to delete evas_object ?
		dlog_print(DLOG_FATAL, "FrameWindow", msg);
		return false;
	}
	return true;
}

View* FrameWindow::GetView(int viewid)
{
	View* pview = findview(viewid);
	baseassert(pview!=NULL);
	return pview;
}

bool FrameWindow::AddView(int viewid, View* view)
{
	baseassert(m_framewnd!= NULL && m_conformant!=NULL);
	if(findview(viewid) != NULL)
	{
		dlog_print(DLOG_FATAL, "FrameWindow", "duplicated view id");
		return false;
	}

	bool created = view->CreateView(m_framewnd, m_conformant);
	if(created == false)
	{
		return false;
	}
	m_viewmap[viewid] = (View*)view;
	return true;
}

void FrameWindow::Show()
{
	evas_object_show(m_framewnd);
}

View* FrameWindow::findview(int viewid)
{
	std::map<int, View*>::iterator it;
	it = m_viewmap.find(viewid);
	if(it!=m_viewmap.end())
	{
		return it->second;
	}
	return NULL;
}

Evas_Object* FrameWindow::createframewindow()
{
	Evas_Object* pframewnd = NULL;
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	pframewnd = elm_win_util_standard_add(PACKAGE, PACKAGE);
	if(pframewnd == NULL)
		throw "fail to create frame window";
	elm_win_conformant_set(pframewnd, EINA_TRUE);
	elm_win_autodel_set(pframewnd, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(pframewnd)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(pframewnd, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(pframewnd, "delete,request", delete_request_cb, this);

	return pframewnd;
}

void FrameWindow::delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	FrameWindow* framewnd = static_cast<FrameWindow*>(data);
	if(framewnd != NULL)
	{
		framewnd->handledeleterequest(obj, event_info);
		return;
	}
	//assert
}
void FrameWindow::handledeleterequest(Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

Evas_Object* FrameWindow::createconformation(Evas_Object* parentwnd)
{
	Evas_Object* pconformation = NULL;
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	pconformation = elm_conformant_add(parentwnd);
	if(pconformation == NULL)
		throw "fail to create conformant window";
	elm_win_indicator_mode_set(parentwnd, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(parentwnd, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(pconformation, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parentwnd, pconformation);
	evas_object_show(pconformation);

	return pconformation;
}



