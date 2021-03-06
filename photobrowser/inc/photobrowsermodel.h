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
#include "common/model.h"
#include "common/mediacontentcontroller.h"




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
	int GetSelectedIndex();
	EXIF& GetExif_of_SelectedContent();

protected:
	bool loadexif(const char* imagefilename);
	void creatspecifics();
	void destroyspecifics();
	void buildcontentslist();

private:
	int m_selectedcontentindex;
	std::vector<MediaContentItem> m_contents;
	EXIF m_exif;
};




#endif /* PHOTOBROWSERMODEL_H_ */
