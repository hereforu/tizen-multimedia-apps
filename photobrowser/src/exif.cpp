/*
 * exif.cpp
 *
 *  Created on: Jul 5, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "exif.h"
#include <stdexcept>


EXIF::EXIF()
:m_data(NULL)
{

}

EXIF::~EXIF()
{

}

/*
 * It extracts tags and tagged data from images with EXIF data and constructs ExifTag_Value vector
 * exif_data_new_from_file fetches ExifData struct from image files.
 * The ExifData object is used to fetch ExifContent struct, so it is taken as a member variable.
 * If ExifData is not created, it is implied that the specified image does not contain EXIF data.
 */
bool EXIF::Open(const char* imagefilename)
{
	m_data = exif_data_new_from_file(imagefilename);
	if(m_data == NULL)
	{
		dlog_print(DLOG_DEBUG, "EXIF", "there is no exif [%s]", imagefilename);
		return false;
	}
	build_contents_entries();
	build_taglist();
	return true;
}
bool EXIF::IsOpened()
{
	return (m_data)?true:false;
}
void EXIF::Close()
{
//	AppTool::Assert(m_data != NULL);
	if(m_data)
	{
		clear_taglist();
		clear_contents_entries();
		exif_data_unref(m_data);
		m_data = NULL;
	}
}

unsigned int EXIF::GetNumTags()
{
	return m_supported_tag_and_values.size();
}
const char* EXIF::GetTagName(int index)
{
	if(index < 0 || index >= m_supported_tag_and_values.size())
		return NULL;
	return m_supported_tag_and_values[index].name.c_str();
}
const char* EXIF::GetValue(int index)
{
	if(index < 0 || index >= m_supported_tag_and_values.size())
		return NULL;
	return m_supported_tag_and_values[index].value.c_str();
}

std::vector<ExifTag_Value>& EXIF::GetTagAndValueList()
{
	return m_supported_tag_and_values;
}



void EXIF::load_ifdname_map()
{
	m_ifdnamemap[EXIF_IFD_0] = "EXIF IFD 0";
	m_ifdnamemap[EXIF_IFD_1] = "EXIF IFD 1";
	m_ifdnamemap[EXIF_IFD_EXIF] = "EXIF IFD EXIF";
	m_ifdnamemap[EXIF_IFD_GPS] = "EXIF IFD GPS";
	m_ifdnamemap[EXIF_IFD_INTEROPERABILITY] = "EXIF IFD INTEROPERABILITY";
}

void EXIF::clear_contents_entries()
{
	for(unsigned int c = 0; c < m_contents.size(); ++c)
	{
		for(unsigned int e = 0; e < m_contents[c].entries.size(); ++e)
		{
			;
	//		exif_entry_unref(m_contents[c].entries[e]);
		}
		m_contents[c].entries.clear();
	//	exif_content_unref(m_contents[c].content);
	}
	m_contents.clear();
}

void EXIF::build_taglist()
{
	const int buflen = 256;
	char buf[buflen]={0,};
	for(unsigned int c = 0; c < m_contents.size(); ++c)
	{
		ExifIfd ifdid = exif_content_get_ifd(m_contents[c].content);
		for(unsigned int e = 0; e < m_contents[c].entries.size(); ++e)
		{
			m_supported_tag_and_values.push_back( ExifTag_Value(
					exif_tag_get_name_in_ifd(m_contents[c].entries[e]->tag, ifdid)
					, exif_entry_get_value(m_contents[c].entries[e], buf, buflen)
			));
		}
	}
}

void EXIF::clear_taglist()
{
	m_supported_tag_and_values.clear();
}

/*
 * build_contents_entries is a function that fetches ExifContent from ExifData and constructs ExifEntry.
 * ExifTagCollection is used for managing them because ExifContent and ExifEntry are in a hierarchical relationship
 */
void EXIF::build_contents_entries()
{
	exif_data_foreach_content(m_data, foreach_content_cb, (void*)this);
}

/*
 * foreach_content is a registered callback function used for relaying ExifContent, which triggers call to 'exif_content_foreach_entry' to extract ExifEntry from ExifContent.
 */
void EXIF::foreach_content(ExifContent* content)
{
	m_contents.push_back(ExifTagCollection(content));
	exif_content_foreach_entry(content, foreach_entry_cb, (void*)this);
}
void EXIF::foreach_entry(ExifEntry* entry)
{
	m_contents.back().entries.push_back(entry);
}
void EXIF::foreach_content_cb(ExifContent* content, void* data)
{
	EXIF* exif = (EXIF*)data;
	exif->foreach_content(content);
}
void EXIF::foreach_entry_cb(ExifEntry* entry, void* data)
{
	EXIF* exif = (EXIF*)data;
	exif->foreach_entry(entry);
}
