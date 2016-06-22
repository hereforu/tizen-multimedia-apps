/*
 * mediacontent.cpp
 *
 *  Created on: Jun 20, 2016
 *      Author: Jason
 */

#include <stdexcept>
#include <string>
#include <glib.h>
#include <stdlib.h>
#include "base.h"
#include "mediacontent.h"

#define MAKE_MEDIATYPE_STRING(addor, media_type) \
	(addor)? std::string(" OR MEDIA_TYPE = ") + AppTool::ToString<int>(media_type) : std::string(" MEDIA_TYPE = ") + AppTool::ToString<int>(media_type)

MediaContent::MediaContent()
	:m_isconnected(false)
{

}

MediaContent::~MediaContent()
{

}

void MediaContent::ConnectDB()
{
	AppTool::Assert(m_isconnected == false);

	int ret = media_content_connect();
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to connect to the media content DB with code:")+AppTool::ToString<int>(ret));
	}
	m_isconnected = true;
}

void MediaContent::DisconnectDB()
{
	AppTool::Assert(m_isconnected == true);

	media_content_disconnect();
	m_isconnected = false;

}

void MediaContent::GetItem(const MediaContentParam& param, std::vector<MediaContentItem>* itemlist)
{
	AppTool::Assert(m_isconnected == true);

	filter_h filter = NULL;
	try
	{
		filter = createfilter(param);
		getitemlist(filter, itemlist);
		destroyfilter(filter);
	}
	catch(const std::runtime_error& e)
	{
		destroyfilter(filter);
	}
}

filter_h MediaContent::createfilter(const MediaContentParam& param)
{
	filter_h filter = NULL;
	int ret;
	if((ret = media_filter_create(&filter)) != MEDIA_CONTENT_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to create filter with code:")+AppTool::ToString<int>(ret));
	}

	media_content_collation_e collate_type = MEDIA_CONTENT_COLLATE_NOCASE;
	std::string condition = createcondition(param.mediatype);
	if((ret = media_filter_set_condition(filter, condition.c_str(), collate_type)) != MEDIA_CONTENT_ERROR_NONE)
	{
		media_filter_destroy(filter);
		throw std::runtime_error(std::string("fail to set condition to the filter with code:")+AppTool::ToString<int>(ret));
	}

	media_content_order_e order_type = (param.isASC)? MEDIA_CONTENT_ORDER_ASC:MEDIA_CONTENT_ORDER_DESC;
	if((ret = media_filter_set_order(filter, order_type, param.order_keyword.c_str(), collate_type)) != MEDIA_CONTENT_ERROR_NONE)
	{
		media_filter_destroy(filter);
		throw std::runtime_error(std::string("fail to set order type to the filter with code:")+AppTool::ToString<int>(ret));
	}

	return filter;
}

bool MediaContent::media_cb(media_info_h media, void *user_data)
{
	char* media_display_name = NULL;
	media_info_get_display_name(media, &media_display_name);

	if(media_display_name)
	{
		std::vector<MediaContentItem>* itemlist = (std::vector<MediaContentItem>*)user_data;
		MediaContentItem Item;
		Item.path = media_display_name;
		itemlist->push_back(Item);
		free(media_display_name);
		media_display_name = NULL;
	}
	return true;
}
void MediaContent::getitemlist(filter_h filter, std::vector<MediaContentItem>* itemlist)
{
	int ret;
	/*
	 * media_info_foreach_media_from_db function is synchronous, and it blocks until the callback has been called for all items or the callback returns false.
	 */
	if((ret = media_info_foreach_media_from_db(filter, media_cb, (void*)itemlist)) != MEDIA_CONTENT_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to media_info_foreach_media_from_db with code:")+AppTool::ToString<int>(ret));
	}
}
void MediaContent::destroyfilter(filter_h& filter)
{
	if(filter)
	{
		media_filter_destroy(filter);
		filter = NULL;
	}
}


std::string MediaContent::createcondition(unsigned int mediatype)
{
	bool addor = false;
	std::string condition("");
	if(mediatype & MC_IMAGE_TYPE)
	{
		condition += MAKE_MEDIATYPE_STRING(addor, MEDIA_CONTENT_TYPE_IMAGE);
		addor = true;
	}
	if(mediatype & MC_VIDEO_TYPE)
	{
		condition += MAKE_MEDIATYPE_STRING(addor, MEDIA_CONTENT_TYPE_VIDEO);
		addor = true;
	}
	if(mediatype & MC_SOUND_TYPE)
	{
		condition += MAKE_MEDIATYPE_STRING(addor, MEDIA_CONTENT_TYPE_SOUND);
		addor = true;
	}
	if(mediatype & MC_MUSIC_TYPE)
	{
		condition += MAKE_MEDIATYPE_STRING(addor, MEDIA_CONTENT_TYPE_MUSIC);
		addor = true;
	}
	return condition;
}


