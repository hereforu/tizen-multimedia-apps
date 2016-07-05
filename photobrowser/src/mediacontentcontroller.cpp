/*
 * mediacontentcontroller.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: merci
 */

#include <stdexcept>
#include <string>
#include <stdlib.h>
#include "base.h"
#include "mediacontentcontroller.h"
#include <dlog.h>


#define MAKE_MEDIATYPE_STRING(addor, media_type) \
	(addor)? std::string(" OR MEDIA_TYPE = ") + AppTool::ToString<int>(media_type) : std::string(" MEDIA_TYPE = ") + AppTool::ToString<int>(media_type)

#undef LOG_TAG
#define LOG_TAG "APP_TAG"


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
	//register db update callback
	if((ret = media_content_set_db_updated_cb(_noti_cb, NULL)) != MEDIA_CONTENT_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to media_content_set_db_updated_db with code:") + AppTool::ToString<int>(ret));
	}
}

void MediaContent::DisconnectDB()
{
	AppTool::Assert(m_isconnected == true);

	// deregister db update callback
	int ret = media_content_unset_db_updated_cb();
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to media_content_unset_db_updated_cb with code:")+AppTool::ToString<int>(ret));
	}
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
		throw std::runtime_error(std::string("fail to GetItem") + e.what());
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
	char* media_file_path = NULL;
	char* media_thumbnail_file_path = NULL;

	media_info_get_display_name(media, &media_display_name);
	media_info_get_file_path(media, &media_file_path);
	media_info_get_thumbnail_path(media, &media_thumbnail_file_path);

	MediaContentItem Item;
	if(media_display_name)
	{
		Item.name = media_display_name;
		free(media_display_name);
	}
	if(media_file_path)
	{
		Item.path = media_file_path;
		free(media_file_path);
	}
	if(media_thumbnail_file_path)
	{
		Item.thumbnail_path = media_thumbnail_file_path;
		free(media_thumbnail_file_path);
	}

	video_meta_h video = NULL;
	media_info_get_video(media, &video);
	if(video)
	{
		video_meta_get_width(video, &Item.width);
		video_meta_get_height(video, &Item.height);
		video_meta_get_duration(video, &Item.duration);
		video_meta_get_bit_rate(video, &Item.video_bitrate);
	}
	audio_meta_h audio = NULL;
	media_info_get_audio(media, &audio);
	if(audio)
	{
		audio_meta_get_channel(audio, &Item.audio_channel);
		audio_meta_get_bit_rate(audio, &Item.audio_bitrate);
		audio_meta_get_bitpersample(audio, &Item.audio_sample);
	}
	std::vector<MediaContentItem>* itemlist = (std::vector<MediaContentItem>*)user_data;
	itemlist->push_back(Item);

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


void MediaContent::_noti_cb(media_content_error_e error, int pid,
         media_content_db_update_item_type_e update_item,
         media_content_db_update_type_e update_type,
         media_content_type_e media_type,
         char *uuid, char *path, char *mime_type, void *user_data)
{
   if (error == MEDIA_CONTENT_ERROR_NONE)
   {
      // Database was successfully updated
      if (path)
         dlog_print(DLOG_INFO, LOG_TAG, "path : %s", path);
      if (uuid)
         dlog_print(DLOG_INFO, LOG_TAG, "uuid : %s", uuid);
      if (mime_type)
         dlog_print(DLOG_INFO, LOG_TAG, "mime_type : %s", mime_type);
   }
}


