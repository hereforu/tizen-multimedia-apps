/*
 * media.h
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 * Description: Media(iamge, audio, video base class).
 */

#ifndef MEDIA_H_
#define MEDIA_H_

class Media
{
public:
	Media();
	virtual ~Media();

	void CreateMedia();

	// get methods
	char** getMediatype();
	char** getFileName();
	char** getFilePath();
	int getFileSize();
	char** getFileDate();
	char** getThumbnailPath();

protected:
	virtual void setMediaInfo() = 0;

private:
	char** m_mediatype;
	char** m_filename;
	char** m_filepath;
	int m_filesize;		// TODO: size unit? byte 단위?
	char** m_filedate; // TODO: check date type in tizen
	char** m_thumbnailpath;
};


#endif /* MEDIA_H_ */
