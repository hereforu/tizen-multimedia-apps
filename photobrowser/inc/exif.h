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

typedef struct _ExifTagCollection
{
	_ExifTagCollection(ExifContent* _content)
	:content(_content)
	{

	}
	ExifContent* content;
	std::vector<ExifEntry*> entries;
}ExifTagCollection;

typedef struct _ExifTag_Value
{
	_ExifTag_Value(const char* _name, const char* _value)
	:name(_name), value(_value)
	{}
	std::string name;
	std::string value;
}ExifTag_Value;
class EXIF
{
public:
	EXIF();
	~EXIF();

	bool Open(const char* imagefilename);
	void Close();
	bool IsOpened();

	unsigned int GetNumTags();
	const char* GetTagName(int index);
	const char* GetValue(int index);

	std::vector<ExifTag_Value>& GetTagAndValueList();


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
	std::vector<ExifTag_Value> m_supported_tag_and_values;
	ExifData* m_data;
	std::vector<ExifTagCollection> m_contents;
	std::map<int, std::string> m_ifdnamemap;


};



#endif /* EXIF_H_ */
