/*
 * media.h
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 * Description: Media(iamge, audio, video base class).
 */

#ifndef MEDIA_H_
#define MEDIA_H_

#include <string>

class Media
{
public:
	Media();
	virtual ~Media();

	void CreateMedia();

	// get methods
	std::string getMediatype(); //enum으로 TODO: sournd, audio, video, image, others
	std::string getFileName();
	std::string getFilePath();
	int getFileSize();
	std::string getFileDate();
	std::string getThumbnailPath();

protected:
	virtual void setMediaInfo() = 0;

private:
	std::string m_mediatype;
	std::string m_filename;
	std::string m_filepath;
	int m_filesize;		// TODO: size unit? byte 단위?
	std::string m_filedate; // TODO: check date type in tizen
	std::string m_thumbnailpath;
};


#endif /* MEDIA_H_ */
