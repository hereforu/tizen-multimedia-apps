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
	string getMediatype(); //enum으로 TODO: sournd, audio, video, image, others
	string getFileName();
	string getFilePath();
	int getFileSize();
	string getFileDate();
	string getThumbnailPath();

protected:
	virtual void setMediaInfo() = 0;

private:
	string m_mediatype;
	string m_filename;
	string m_filepath;
	int m_filesize;		// TODO: size unit? byte 단위?
	string m_filedate; // TODO: check date type in tizen
	string m_thumbnailpath;
};


#endif /* MEDIA_H_ */
