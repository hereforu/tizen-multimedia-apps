/*
 * optionview.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: Jason
 */

#include "optionview.h"
#include "multimediaapp.h"
#include <stdexcept>
#include "mediacontentcontroller.h"




OptionView::OptionView()
{

}

OptionView::~OptionView()
{

}

const char* OptionView::getedcfilename()
{
	return "Select One";
}

void OptionView::decorateview(Evas_Object* box)
{
	try
	{
		m_list.Create(box, choose_one_cb, (void*)this);
		setinfo_tolist(m_list, getmodel()->GetSelectedContent());
		m_list.SelectItem(0);
		m_btnpack.Create(box);
		add_defaultbtns(m_btnpack);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "InfoView", e.what());
	}
}

void OptionView::destroyremains()
{
	m_list.Destroy();
	m_btnpack.Destroy();
}


void OptionView::add_defaultbtns(ButtonPack& btnpack)
{
	std::vector<BTPackParam> functionbtn_params;
	functionbtn_params.push_back(BTPackParam(NULL, "images/exit.png", OptionView::clicked_prev_cb, (void*)this));
	btnpack.AddPacksHorizontally(functionbtn_params);
}

void OptionView::setinfo_tolist(ListCtrl& list, const MediaContentItem& content)
{
	try
	{
		TranscodingOptionType type = getmodel()->GetSelectedTypeofOptions();
		std::vector<TranscodingOption>& options = getmodel()->GetOptionSet(type);

		std::vector<GenCtrlItem> items;
		for(unsigned int i = 0; i < options.size(); ++i)
		{
			items.push_back(GenCtrlItem(options[i].id, options[i].name.c_str(), "no media", getresiconpath(options[i].icon.c_str()).c_str()));
		}
		list.AppendItems(items);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "InfoView", e.what());
	}
}
std::string OptionView::getresiconpath(const char* iconname)
{
	return std::string(app_get_resource_path()) + iconname;
}
TranscoderModel* OptionView::getmodel()
{
	TranscoderModel* model = static_cast<TranscoderModel*>(MODEL);
	AppTool::Assert(model != NULL);
	return model;
}


void OptionView::UpdateView()
{

}

void OptionView::chooseone(int id)
{
	TranscodingOptionType type = getmodel()->GetSelectedTypeofOptions();
	getmodel()->SelectOption(type, id);
	MOVE_PREVVIEW;
}

void OptionView::clicked_prev_cb(void *data, Evas_Object *obj, void *event_info)
{
	MOVE_PREVVIEW;
}
void OptionView::choose_one_cb(void *data, int index)
{

	OptionView* view = (OptionView*)data;
	view->chooseone(index);

}


