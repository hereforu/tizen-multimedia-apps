/*
 * mediacontent.h
 *
 *  Created on: Jun 20, 2016
 *      Author: Jason
 */

#ifndef MEDIACONTENT_H_
#define MEDIACONTENT_H_

#include <vector>
#include <string>
#include <media_content.h>

typedef enum _MediaContentMediaType
{
	MC_IMAGE_TYPE   = 0x01,
	MC_VIDEO_TYPE   = 0x02,
	MC_SOUND_TYPE   = 0x04,
	MC_MUSIC_TYPE   = 0x08,
	MC_ALL_TYPE = 0x0F
}MediaContentMediaType;

typedef struct _MediaContentParam
{
	_MediaContentParam()
	:mediatype(MC_ALL_TYPE), order_keyword(MEDIA_DISPLAY_NAME), isASC(true)
	{
	}
	unsigned int mediatype;
	std::string order_keyword;
	bool isASC;
}MediaContentParam;

typedef struct _MediaContentItem
{
	std::string path;
}MediaContentItem;

class MediaContent
{
public:


	MediaContent();
	~MediaContent();

	void ConnectDB();
	void DisconnectDB();
	bool IsConnected(){return m_isconnected;}

	void GetItem(const MediaContentParam& param, std::vector<MediaContentItem>* itemlist);

private:
	static bool media_cb(media_info_h media, void *user_data);
	filter_h createfilter(const MediaContentParam& param);
	void getitemlist(filter_h filter, std::vector<MediaContentItem>* itemlist);
	void destroyfilter(filter_h& filter);
	std::string createcondition(unsigned int mediatype);
private:
	bool m_isconnected;

};



#endif /* MEDIACONTENT_H_ */
