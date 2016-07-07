/*
 * exifview.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */


#include "exifview.h"
#include "multimediaapp.h"
#include <stdexcept>


ExifView::ExifView()
{

}

ExifView::~ExifView()
{

}

const char* ExifView::getedcfilename()
{
	return getmodel()->GetSelectedContent().name.c_str();
}

void ExifView::decorateview(Evas_Object* box)
{
	try
	{
		m_list.Create(box, change_optionview_cb, (void*)this, ListCtrl::SMALL_ITEM_SIZE);
		setinfo_tolist(m_list, getmodel()->GetExif_of_SelectedContent());
		m_btnpack.Create(box);
		add_defaultbtns(m_btnpack);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "ExifView", e.what());
	}
}

void ExifView::destroyremains()
{
	m_list.Destroy();
	m_btnpack.Destroy();
}


void ExifView::add_defaultbtns(ButtonPack& btnpack)
{
	std::vector<BTPackParam> functionbtn_params;
	functionbtn_params.push_back(BTPackParam(NULL, "images/exit.png", ExifView::clicked_prev_cb, (void*)this));
	functionbtn_params.push_back(BTPackParam(NULL, "images/play.png", ExifView::clicked_play_cb, (void*)this));
	btnpack.AddPacksHorizontally(functionbtn_params);
}

//TODO
void ExifView::setinfo_tolist(ListCtrl& list, EXIF& exif)
{
	std::vector<GenCtrlItem> items;

	std::vector<ExifTagID_Name>& taglist = exif.GetSupportedTagList();
	for(int i = 0; i < taglist.size(); ++i)
	{
		std::string value = taglist[i].name + " : ";
		value += exif.GetValue(taglist[i]);
		items.push_back(GenCtrlItem(i, value.c_str(), "no media", getresiconpath("images/tag.png").c_str()));
		dlog_print(DLOG_DEBUG, "EXIF", "%s", value.c_str());
	}
	list.AppendItems(items);
}

std::string ExifView::getresiconpath(const char* iconname)
{
	return std::string(app_get_resource_path()) + iconname;
}



void ExifView::UpdateView()
{
	m_list.RemoveAllItems();
	setinfo_tolist(m_list, getmodel()->GetExif_of_SelectedContent());
}

void ExifView::change_optionview(int id)
{
	//TODO
}
void ExifView::move_prev()
{
	MOVE_PREVVIEW;
}
void ExifView::play()
{
	MOVE_NEXTVIEW;
}

void ExifView::clicked_prev_cb(void *data, Evas_Object *obj, void *event_info)
{
	ExifView* view = (ExifView*)data;
	view->move_prev();
}
void ExifView::clicked_play_cb(void *data, Evas_Object *obj, void *event_info)
{
	ExifView* view = (ExifView*)data;
	view->play();

}

void ExifView::change_optionview_cb(void *data, int id)
{
	ExifView* view = (ExifView*)data;
	view->change_optionview(id);
}





