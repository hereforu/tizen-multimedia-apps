/*
 * transcodermodel.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */
#include "common/base.h"
#include "transcodermodel.h"
#include "common/mediacontentcontroller.h"
#include <stdexcept>

const int no_selected_content = -1;

TranscoderModel::TranscoderModel()
:m_selectedcontentindex(no_selected_content), m_selectedoptiontype(VIDEO_CODEC_OPTION)
{

}

TranscoderModel::~TranscoderModel()
{

}

void TranscoderModel::RebuildContentsList()
{
	buildcontentslist();
}

void TranscoderModel::SetSelectedContentIndex(int index)
{
	assert_ifnot(0 <= index && index < m_contents.size());
	m_selectedcontentindex = index;
}

const MediaContentItem& TranscoderModel::GetSelectedContent()
{
	assert_ifnot(m_selectedcontentindex != no_selected_content);
	return m_contents[m_selectedcontentindex];
}


const std::vector<MediaContentItem>& TranscoderModel::GetContentsList()
{
	return m_contents;
}

void TranscoderModel::buildcontentslist()
{
	MediaContentParam param;
	param.mediatype = MC_VIDEO_TYPE;
	MediaContent db;
	m_contents.clear();
	try
	{
		db.ConnectDB();
		db.GetItem(param, &m_contents);
		db.DisconnectDB();
		m_selectedcontentindex = no_selected_content;//reset
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "TranscoderModel", e.what());
	}
}


void TranscoderModel::ResetOptions()
{
	m_options.ResetOptions();
}

void TranscoderModel::SelectTypeofOptions(TranscodingOptionType type)
{
	m_selectedoptiontype = type;
}

TranscodingOptionType TranscoderModel::GetSelectedTypeofOptions()
{
	return m_selectedoptiontype;
}

std::vector<TranscodingOption>& TranscoderModel::GetOptionSet(TranscodingOptionType type)
{
	return m_options.GetOptionSet(type);
}
void TranscoderModel::SelectOption(TranscodingOptionType type, unsigned int option)
{
	m_options.SelectOption(type, option);
}
unsigned int TranscoderModel::GetSelectedOption(TranscodingOptionType type)
{
	return m_options.GetSelectedOption(type);
}
const char* TranscoderModel::GetOptionName(TranscodingOptionType type, unsigned int id)
{
	return m_options.GetOptionName(type, id);
}
void TranscoderModel::creatspecifics()
{
	buildcontentslist();
}

void TranscoderModel::destroyspecifics()
{

}
