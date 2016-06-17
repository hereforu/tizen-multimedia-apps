/*
 * mediacontentcontroller.cpp
 *
 *  Created on: Jun 18, 2016
 *      Author: merci
 */


#include "mediacontentcontroller.h"
using namespace MediaContentController;



int MediaContentController::getContentsByCondition(std::vector<string> &Audiolist)
{
	int ret = MEDIA_CONTENT_ERROR_NONE;
	filter_h filter = NULL;

	ret = media_content_connect();
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "Connection is failed : %d", ret);
	}

	ret = media_filter_create(&filter);
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "Fail to create filter : %d", ret);
	}

	ret = getContentFromDB(filter, Audiolist);

	ret = media_filter_destroy(filter);
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "Fail to destroy filter : %d", ret);
	}

	ret = media_content_disconnect();
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "Disconnection is failed : %d", ret);
	}

	return ret;
}

int MediaContentController::getContentFromDB(filter_h filter, std::vector<string> &Audiolist)
{
	int ret = MEDIA_CONTENT_ERROR_NONE;
	media_info_h media_handle = NULL;
	GList *content_list = NULL;
	char *media_path = NULL;
	ret = media_filter_set_condition(filter, "MEDIA_TYPE = MEDIA_CONTENT_TYPE_SOUND OR MEDIA_TYPE = MEDIA_CONTENT_TYPE_MUSIC", MEDIA_CONTENT_COLLATE_DEFAULT);
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "media_filter_set_condition() is failed : %d", ret);
	}
	ret = media_info_foreach_media_from_db(filter, gallery_media_item_cb, &content_list);
	if(ret != MEDIA_CONTENT_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "media_info_foreach_media_from_db() is failed : %d", ret);
	}

	if(content_list)
	{
		int len = g_list_length(content_list);
		for(int i=0; i<len; i++){
			media_info_h media_handle = (media_info_h) g_list_nth_data(content_list, i);
			ret = media_info_get_file_path(media_handle, &media_path);
			if(ret != MEDIA_CONTENT_ERROR_NONE)
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "media_info_get_file_path() is failed : %d", ret);
			}
			Audiolist.push_back(std::string(media_path));
			if(media_path) free(media_path);
		}
	}
	g_list_free(content_list);
	free(media_handle);

	return ret;
}

bool MediaContentController::gallery_media_item_cb(media_info_h media, void *user_data)
{
   media_info_h new_media = NULL;
   media_info_clone(&new_media, media);

   GList **list = (GList**)user_data; // Include glib.h for this value
   *list = g_list_append(*list, new_media);

   return true;
}

bool MediaContentController::checkFileExistence(char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

