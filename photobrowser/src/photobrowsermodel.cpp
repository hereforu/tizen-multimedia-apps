/*
 * photobrowsermodel.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */
#include "base.h"
#include "photobrowsermodel.h"
#include "mediacontentcontroller.h"
#include <stdexcept>

const int no_selected_content = -1;

PhotoBrowserModel::PhotoBrowserModel()
:m_selectedcontentindex(no_selected_content)
{

}

PhotoBrowserModel::~PhotoBrowserModel()
{

}

void PhotoBrowserModel::RebuildContentsList()
{
	buildcontentslist();
}

void PhotoBrowserModel::SetSelectedContentIndex(int index)
{
	AppTool::Assert(0 <= index && index < m_contents.size());
	m_selectedcontentindex = index;
}

bool PhotoBrowserModel::SetSelectedContentIndexifExif(int index)
{
	AppTool::Assert(0 <= index && index < m_contents.size());

	if(loadexif(m_contents[index].path.c_str()))
	{
		m_selectedcontentindex = index;
		return true;
	}
	return false;
}

const MediaContentItem& PhotoBrowserModel::GetSelectedContent()
{
	AppTool::Assert(m_selectedcontentindex != no_selected_content);
	return m_contents[m_selectedcontentindex];
}

const std::vector<MediaContentItem>& PhotoBrowserModel::GetContentsList()
{
	return m_contents;
}

EXIF& PhotoBrowserModel::GetExif_of_SelectedContent()
{
	return m_exif;
}

bool PhotoBrowserModel::loadexif(const char* imagefilename)
{
	if(m_exif.IsCreated())
		m_exif.Destroy();
	return m_exif.Create(imagefilename);
}

void PhotoBrowserModel::buildcontentslist()
{
	MediaContentParam param;
	param.mediatype = MC_IMAGE_TYPE;
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
		dlog_print(DLOG_ERROR, "PhotoBrowserModel", e.what());
	}
}


bool PhotoBrowserModel::creatspecifics()
{
	buildcontentslist();
	return true;
}

void PhotoBrowserModel::destroyspecifics()
{

}
