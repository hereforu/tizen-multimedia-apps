/*
 * exif.cpp
 *
 *  Created on: Jul 5, 2016
 *      Author: Jason
 */

#include "base.h"
#include "exif.h"
#include <stdexcept>


EXIF::EXIF()
:m_data(NULL)
{

}

EXIF::~EXIF()
{

}

bool EXIF::Create(const char* imagefilename)
{
//	AppTool::Assert(m_data == NULL);
	m_data = exif_data_new_from_file(imagefilename);
	if(m_data == NULL)
	{
		dlog_print(DLOG_DEBUG, "EXIF", "there is no exif [%s]", imagefilename);
		return false;
	}
	dlog_print(DLOG_DEBUG, "EXIF", "exif_data_new_from_filep m_data[%p]", m_data);
	build_contents_entries();
	build_taglist();
	return true;
}
bool EXIF::IsCreated()
{
	return (m_data)?true:false;
}
void EXIF::Destroy()
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

std::vector<ExifTagID_Name>& EXIF::GetSupportedTagList()
{
	return m_supportedtags;
}

std::string EXIF::GetValue(ExifTagID_Name& tag)
{
	const int buflen = 256;
	char buf[buflen]={0,};
	return std::string(exif_entry_get_value(tag.htag, buf, buflen));
}

void EXIF::UpdateValue(int tagid, const char* value)
{

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
	for(unsigned int c = 0; c < m_contents.size(); ++c)
	{
		ExifIfd ifdid = exif_content_get_ifd(m_contents[c].content);
		for(unsigned int e = 0; e < m_contents[c].entries.size(); ++e)
		{
			m_supportedtags.push_back( ExifTagID_Name( m_contents[c].entries[e],
					exif_tag_get_name_in_ifd(m_contents[c].entries[e]->tag, ifdid)
			));
		}
	}
}

void EXIF::clear_taglist()
{
	m_supportedtags.clear();
}

void EXIF::build_contents_entries()
{
	exif_data_foreach_content(m_data, foreach_content_cb, (void*)this);
}

void EXIF::foreach_content(ExifContent* content)
{
	m_contents.push_back(ExifC(content));
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
