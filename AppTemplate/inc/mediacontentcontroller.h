/*
 * mediacontentcontroller.h
 *
 *  Created on: Jun 9, 2016
 *      Author: merci
 */

#ifndef MEDIACONTENTCONTROLLER_H_
#define MEDIACONTENTCONTROLLER_H_

#include <glib.h>
#include <media_content.h>
#include <vector>
#include <dlog.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>

using namespace std;

#define BUF_LEN 255





typedef struct media_info_s * media_info_h;
typedef bool(* media_info_cb)(media_info_h media, void *user_data);

namespace MediaContentController
{

	static int getContentsByCondition(std::vector<string> &Audiolist);
	static bool checkFileExistence(char *path);

	int getContentFromDB(filter_h filter, std::vector<string> &Audiolist);
	bool gallery_media_item_cb(media_info_h media, void *user_data);

};


#endif /* MEDIACONTENTCONTROLLER_H_ */
