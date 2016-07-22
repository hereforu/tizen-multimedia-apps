/*
 * imagelistview.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */


#include "ImageListView.h"
#include "common/multimediaapp.h"
#include <stdexcept>
#include "exifcreator.h"



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
	((PhotoBrowserModel*)getmodel())->RebuildContentsList();
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
	const std::vector<MediaContentItem>& contents = ((PhotoBrowserModel*)getmodel())->GetContentsList();
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
	if(((PhotoBrowserModel*)getmodel())->SetSelectedContentIndexifExif(id))
	{
		MOVE_NEXTVIEW;
	}
	else
	{
		((PhotoBrowserModel*)getmodel())->SetSelectedContentIndex(id);
		showpopup("EXIF가 없는 이미지 파일입니다! EXIF를 만들어 이미지 파일에 삽입하시겠습니까?");
	}
}

void ImageListView::addexif_yes()
{
	MediaContentItem content = ((PhotoBrowserModel*)getmodel())->GetSelectedContent();
	EXIFCreator ec;
	ec.Create(content.path.c_str());
	ec.AddResolution(content.width, content.height);
	ec.AddComment("created by sogang university");
	ec.WriteExif();
	ec.Destroy();
	evas_object_hide(m_popup);
}

void ImageListView::addexif_no()
{
	evas_object_hide(m_popup);
}
void ImageListView::popuptimeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_hide(obj);
}

//TODO: need to manage memory leaks
void ImageListView::clicked_exit_cb(void *data, Evas_Object *obj, void *event_info)
{
	MOVE_PREVVIEW;
}

void ImageListView::select_image_cb(void *data, int id)
{
	ImageListView* view = (ImageListView*)data;
	view->gotonextview(id);
}
void ImageListView::clicked_addexif_yes_cb(void *data, Evas_Object *obj, void *event_info)
{
	ImageListView* view = (ImageListView*)data;
	view->addexif_yes();
}
void ImageListView::clicked_addexif_no_cb(void *data, Evas_Object *obj, void *event_info)
{
	ImageListView* view = (ImageListView*)data;
	view->addexif_no();
}

Evas_Object* ImageListView::createpopup(Evas_Object* box)
{
	Evas_Object* popup = elm_popup_add(box);
	if(popup == NULL)
	{
		throw std::runtime_error("fail to create popup");  // 이 throw를 받아주는 데는 어디?
	}
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	elm_object_style_set(popup, "default");
//	evas_object_smart_callback_add(popup, "timeout", ImageListView::popuptimeout_cb, NULL);
	Evas_Object* yesbtn = createpopupbtns(popup, "images/yes.png", ImageListView::clicked_addexif_yes_cb);
	Evas_Object* nobtn = createpopupbtns(popup, "images/no.png", ImageListView::clicked_addexif_no_cb);
	elm_object_part_content_set(popup, "button1", yesbtn);
	elm_object_part_content_set(popup, "button2", nobtn);
	return popup;
}

Evas_Object* ImageListView::createpopupbtns(Evas_Object* popup, const char* iconpath, Evas_Smart_Cb func_cb)
{
	Evas_Object* btn = elm_button_add(popup);
	if(btn==NULL)
	{
		throw std::runtime_error("fail to create popup buttons");
	}
	std::string resource_path = app_get_resource_path();
	resource_path += iconpath;
	Evas_Object *ic;
	ic = elm_icon_add(btn);
	elm_image_file_set(ic, resource_path.c_str(), NULL);
	elm_object_part_content_set(btn, "icon", ic);
	elm_object_style_set(btn,"circle");
	evas_object_smart_callback_add(btn, "clicked", func_cb, (void*)this);
	return btn;
}

void ImageListView::showpopup(const char* message, int timeout)
{
	elm_object_part_text_set(m_popup, "default", message);
	evas_object_show(m_popup);
}



