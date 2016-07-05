/*
 * imagelistview.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */


#include "ImageListView.h"
#include "multimediaapp.h"
#include <stdexcept>



ImageListView::ImageListView()
:m_popup(NULL)

{

}

ImageListView::~ImageListView()
{

}

const char* ImageListView::getedcfilename()
{
	return "Select a Image";
}

void ImageListView::UpdateView()
{
	getmodel()->RebuildContentsList();
	m_list.RemoveAllItems();
	buildthelist(m_list);
}


void ImageListView::decorateview(Evas_Object* box)
{
	try
	{
		m_list.Create(box, select_image_cb, (void*)this);
		buildthelist(m_list);
		m_popup = createpopup(box);
		m_btnpack.Create(box);
		add_defaultbtns(m_btnpack);

	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "EXIF", e.what());
	}
}


void ImageListView::add_defaultbtns(ButtonPack& btnpack)
{
	std::vector<BTPackParam> functionbtn_params;
	functionbtn_params.push_back(BTPackParam(NULL, "images/exit.png", ImageListView::clicked_exit_cb, (void*)this));
	btnpack.AddPacksHorizontally(functionbtn_params);
}


void ImageListView::destroyremains()
{
	m_list.Destroy();
	m_btnpack.Destroy();
	evas_object_del(m_popup);
	m_popup = NULL;
}
void ImageListView::PrintExif(const char* imagefilename)
{
	EXIF* exif = new EXIF;
	dlog_print(DLOG_DEBUG, "EXIF", "EXIF: %s", imagefilename);
	if(exif->Create(imagefilename))
	{
		std::vector<ExifTagID_Name>& taglist = exif->GetSupportedTagList();
		for(int i = 0; i < taglist.size(); ++i)
		{

			std::string value = exif->GetValue(taglist[i]);
			dlog_print(DLOG_DEBUG, "EXIF", "Tag: %s, Value:%s", taglist[i].name.c_str(), value.c_str());
		}
		exif->Destroy();
	}
	delete exif;
}

void ImageListView::buildthelist(ListCtrl& list)
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

void ImageListView::gotonextview(int id)
{
	if(getmodel()->SetSelectedContentIndexifExif(id))
		MOVE_NEXTVIEW;
	else
	{
		; //TODO: no exif popup
	}
}
void ImageListView::popuptimeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_hide(obj);
}

//TODO: need to manage memory leaks
void ImageListView::clicked_exit_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

void ImageListView::select_image_cb(void *data, int id)
{
	ImageListView* view = (ImageListView*)data;
	view->gotonextview(id);
}

Evas_Object* ImageListView::createpopup(Evas_Object* box)
{
	Evas_Object* popup = elm_popup_add(box);
	if(popup == NULL)
	{
		throw std::runtime_error("fail to create popup");  // 이 throw를 받아주는 데는 어디?
	}
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	elm_object_style_set(popup, "toast");
	evas_object_smart_callback_add(popup, "timeout", ImageListView::popuptimeout_cb, NULL);
	return popup;
}

void ImageListView::showpopup(const char* message, int timeout = 3)
{
	elm_popup_timeout_set(m_popup, (double)timeout);
	elm_object_part_text_set(m_popup, "default", message);
	evas_object_show(m_popup);
}



