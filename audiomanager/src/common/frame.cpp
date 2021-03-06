/*
 * frame.cpp
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "common/frame.h"
#include <stdexcept>


FrameWindow::FrameWindow()
:m_framewnd(NULL), m_conformant(NULL),  m_naviframe(NULL), m_currentviewindex(-1)
{

}

FrameWindow::~FrameWindow()
{
	//delete frame window & conformant
	deleteallviews();
}

void FrameWindow::Create()
{
	try
	{
		m_framewnd = createframewindow();
		m_conformant = createconformation(m_framewnd);
		m_naviframe = createnaviframe(m_conformant);
	}
	catch(const std::runtime_error& e)
	{
		Destroy();
		throw std::runtime_error(std::string("fail to create the frame wnd because ") +  e.what());
	}
}

void FrameWindow::Destroy()
{

}

void FrameWindow::AddView(View* view)
{
	AppTool::Assert(m_framewnd!= NULL && m_conformant!=NULL);
	m_views.push_back(view);

}

View* FrameWindow::GetCurrentView()
{
	return m_views[m_currentviewindex];
}

void FrameWindow::ActivateFirstView()
{
	if(m_views.size()==0 || m_currentviewindex != -1)
	{
		dlog_print(DLOG_DEBUG, "FrameWindow", "no view or m_currentviewindex is -1");
		return;
	}

	try
	{
		pushview(m_views[0]);
		m_currentviewindex = 0;
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_FATAL, "FrameWindow", e.what());
	}
}

void FrameWindow::MoveNextView()
{
	try
	{
		unsigned int num_views = m_views.size();
		if(m_currentviewindex+1 < num_views)
		{
			pushview(m_views[m_currentviewindex+1]);
			{
				++m_currentviewindex;
			}
		}
		else
			dlog_print(DLOG_FATAL, "FrameWindow", "No more views");
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_FATAL, "FrameWindow", e.what());
	}
}

void FrameWindow::MovePrevView()
{
	if(m_currentviewindex > 0)
	{
		popview(m_views[m_currentviewindex]);
		--m_currentviewindex;

		m_views[m_currentviewindex]->UpdateView();
	}
	else
	{
		dlog_print(DLOG_FATAL, "FrameWindow", "this is the first view");
		elm_win_lower(m_framewnd);
	}
}

void FrameWindow::Show()
{
	evas_object_show(m_framewnd);
}
void FrameWindow::pushview(View* view)
{
	if(view->IsCreated())
	{
		dlog_print(DLOG_FATAL, "FrameWindow", "this view is already activated");
		AppTool::Assert(false);

	}
	view->Create(m_naviframe, m_conformant);
}

void FrameWindow::popview(View* view)
{
	if(!view->IsCreated())
	{
		dlog_print(DLOG_FATAL, "FrameWindow", "this view is already deactivated");
		AppTool::Assert(false);

	}
	view->Destroy();
}
void FrameWindow::deleteallviews()
{
	for(unsigned int i= 0; i < m_views.size(); ++i)
	{
		delete m_views[i];
	}
	m_views.clear();
}

Evas_Object* FrameWindow::createframewindow()
{
	Evas_Object* pframewnd = NULL;
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	pframewnd = elm_win_util_standard_add(PACKAGE, PACKAGE);
	if(pframewnd == NULL)
	{
		throw std::runtime_error("fail to create frame window");
	}
	elm_win_conformant_set(pframewnd, EINA_TRUE);
	elm_win_autodel_set(pframewnd, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(pframewnd)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(pframewnd, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(pframewnd, "delete,request", delete_request_cb, this);
	eext_object_event_callback_add(pframewnd, EEXT_CALLBACK_BACK, on_backkeydown_cb, this);

	return pframewnd;
}

void FrameWindow::on_backkeydown_cb(void *data, Evas_Object *obj, void *event_info)
{
	FrameWindow* framewnd = static_cast<FrameWindow*>(data);
	if(framewnd == NULL)
		return;

	framewnd->MovePrevView();
}
void FrameWindow::delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	FrameWindow* framewnd = static_cast<FrameWindow*>(data);
	if(framewnd != NULL)
	{
		framewnd->handledeleterequest(obj, event_info);

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
	{
		throw std::runtime_error("fail to create conformant window");
	}
	elm_win_indicator_mode_set(parentwnd, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(parentwnd, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(pconformation, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parentwnd, pconformation);
	evas_object_show(pconformation);
	return pconformation;
}

Evas_Object* FrameWindow::createnaviframe(Evas_Object* conformant)
{
	Evas_Object* pNaviframe = elm_naviframe_add(conformant);
	if(pNaviframe == NULL)
	{
		throw std::runtime_error("fail to create navi frame");
	}
	elm_object_part_content_set(conformant, "elm.swallow.content", pNaviframe);
	elm_object_content_set(conformant, pNaviframe);
	return pNaviframe;
}



