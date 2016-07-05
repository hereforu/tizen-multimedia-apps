/*
 * photobrowsermodel.h
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */

#ifndef PHOTOBROWSERMODEL_H_
#define PHOTOBROWSERMODEL_H_

#include <string>
#include "exif.h"
#include "model.h"
#include "mediacontentcontroller.h"




class PhotoBrowserModel : public Model
{
public:
	PhotoBrowserModel();
	~PhotoBrowserModel();

	void RebuildContentsList();
	const std::vector<MediaContentItem>& GetContentsList();
	void SetSelectedContentIndex(int index);
	bool SetSelectedContentIndexifExif(int index);
	const MediaContentItem& GetSelectedContent();
	EXIF& GetExif_of_SelectedContent();

protected:
	bool loadexif(const char* imagefilename);
	bool creatspecifics();
	void destroyspecifics();
	void buildcontentslist();

private:
	int m_selectedcontentindex;
	std::vector<MediaContentItem> m_contents;
	EXIF m_exif;
};




#endif /* PHOTOBROWSERMODEL_H_ */
