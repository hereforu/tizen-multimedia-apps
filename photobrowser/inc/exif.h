/*
 * exif.h
 *
 *  Created on: Jul 5, 2016
 *      Author: Jason
 */

#ifndef EXIF_H_
#define EXIF_H_

#include <string>
#include <vector>
#include <map>
#include <libexif/exif-data.h>

typedef ExifEntry* TagHandle;

typedef struct _ExifTagID_Name
{
	_ExifTagID_Name(TagHandle _htag, const char* _name)
	:htag(_htag), name(_name)
	{}
	TagHandle htag;
	std::string name;
}ExifTagID_Name;


typedef struct _ExifC
{
	_ExifC(ExifContent* _content)
	:content(_content)
	{

	}
	ExifContent* content;
	std::vector<ExifEntry*> entries;
}ExifC;


class EXIF
{
public:
	EXIF();
	~EXIF();

	bool Create(const char* imagefilename);
	void Destroy();
	bool IsCreated();

	std::vector<ExifTagID_Name>& GetSupportedTagList();
	std::string GetValue(ExifTagID_Name& tag);
	void UpdateValue(int tagid, const char* value);


private:
	void load_ifdname_map();
	void clear_contents_entries();
	void build_contents_entries();
	void build_taglist();
	void clear_taglist();

	void foreach_content(ExifContent* content);
	void foreach_entry(ExifEntry* entry);

	static void foreach_content_cb(ExifContent* content, void* data);
	static void foreach_entry_cb(ExifEntry* entry, void* data);

private:
	std::vector<ExifTagID_Name> m_supportedtags;
	ExifData* m_data;
	std::vector<ExifC> m_contents;
	std::map<int, std::string> m_ifdnamemap;


};



#endif /* EXIF_H_ */
