/*
 * videolistview.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */


#include "VideoListView.h"
#include "multimediaapp.h"
#include <stdexcept>



VideoListView::VideoListView()
:m_popup(NULL)

{

}

VideoListView::~VideoListView()
{

}

const char* VideoListView::getedcfilename()
{
	return "Select a Video";
}

void VideoListView::UpdateView()
{
	getmodel()->RebuildContentsList();
	getmodel()->ResetOptions();
	m_list.RemoveAllItems();
	buildthelist(m_list);
}


void VideoListView::decorateview(Evas_Object* box)
{
	try
	{
		m_list.Create(box, select_video_cb, (void*)this);
		buildthelist(m_list);
		m_popup = createpopup(box);
		m_btnpack.Create(box);
		add_defaultbtns(m_btnpack);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "VideoListView", e.what());
	}
}


void VideoListView::add_defaultbtns(ButtonPack& btnpack)
{
	std::vector<BTPackParam> functionbtn_params;
	functionbtn_params.push_back(BTPackParam(NULL, "images/exit.png", VideoListView::clicked_exit_cb, (void*)this));
	btnpack.AddPacksHorizontally(functionbtn_params);
}


void VideoListView::destroyremains()
{
	m_list.Destroy();
	m_btnpack.Destroy();
	evas_object_del(m_popup);
	m_popup = NULL;
}

void VideoListView::buildthelist(ListCtrl& list)
{
	const std::vector<MediaContentItem>& contents = getmodel()->GetContentsList();
	std::vector<GenCtrlItem> items;
	for(int i= 0;i < contents.size(); ++i)
	{
		GenCtrlItem item(i, contents[i].name.c_str(), contents[i].path.c_str(), contents[i].thumbnail_path.c_str());
		items.push_back(item);
	}
	list.AppendItems(items);
}

void VideoListView::gotonextview(int id)
{
	getmodel()->SetSelectedContentIndex(id);
	MOVE_NEXTVIEW;
}
void VideoListView::popuptimeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_hide(obj);
}

//TODO: need to manage memory leaks
void VideoListView::clicked_exit_cb(void *data, Evas_Object *obj, void *event_info)
{
	MOVE_PREVVIEW;
}

void VideoListView::select_video_cb(void *data, int id)
{
	VideoListView* view = (VideoListView*)data;
	view->gotonextview(id);
}

Evas_Object* VideoListView::createpopup(Evas_Object* box)
{
	Evas_Object* popup = elm_popup_add(box);
	if(popup == NULL)
	{
		throw std::runtime_error("fail to create popup");  // 이 throw를 받아주는 데는 어디?
	}
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	elm_object_style_set(popup, "toast");
	evas_object_smart_callback_add(popup, "timeout", VideoListView::popuptimeout_cb, NULL);
	return popup;
}

void VideoListView::showpopup(const char* message, int timeout)
{
	elm_popup_timeout_set(m_popup, (double)timeout);
	elm_object_part_text_set(m_popup, "default", message);
	evas_object_show(m_popup);
}



