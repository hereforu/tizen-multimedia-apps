/*
 * media.cpp
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 */

#include "Media.h"
#include <stdexcept>

Media::Media()
	:m_mediatype(NULL), m_filename(NULL), m_filepath(NULL), m_filesize(0), m_filedate(NULL), m_thumbnailpath(NULL)
{

}
Media::~Media()
{

}

void Media::CreateMedia()
{
	setMediaInfo();
}
