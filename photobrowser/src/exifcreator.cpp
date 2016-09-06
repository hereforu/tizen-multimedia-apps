/*
 * exifcreator.cpp
 *
 *  Created on: Jul 6, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "exifcreator.h"
#include <stdexcept>
#include <iostream>
#include <fstream>

EXIFCreator::EXIFCreator()
:m_data(NULL)
{

}
EXIFCreator::~EXIFCreator()
{

}

bool EXIFCreator::Create(const char* jpegfilename)
{
	if((m_data = create_exifdata())==NULL)
		return false;
	m_jpegfilename = jpegfilename;
	return true;
}

bool EXIFCreator::IsCreated()
{
	return (m_data)?true:false;
}

void EXIFCreator::Destroy()
{
	if(!IsCreated())
		return;

	exif_data_unref(m_data);
}

void EXIFCreator::AddResolution(int width, int height)
{
	ExifEntry* width_entry = get_or_create_tag_ifnone(m_data, EXIF_IFD_0, EXIF_TAG_IMAGE_WIDTH);
	setdata(width_entry->data, width_entry->format, width);
	ExifEntry* length_entry = get_or_create_tag_ifnone(m_data, EXIF_IFD_0, EXIF_TAG_IMAGE_LENGTH);
	setdata(length_entry->data, length_entry->format, height);
}
void EXIFCreator::AddComment(const char* text)
{
	int len = strlen(text);
	char* mem = new char[8+len];
	memcpy(mem, "ASCII\0\0\0", 8);
	memcpy(mem+8, text, len);
	ExifEntry* comment_entry = get_or_create_tag_with_memory_ifnone(m_data, EXIF_IFD_EXIF, EXIF_TAG_USER_COMMENT, 8+len);
	setdata(comment_entry->data, mem, 8+len);
	delete[] mem;
}

bool EXIFCreator::WriteExif()
{
	unsigned char* exif_data = NULL;
	unsigned int exif_data_len = 0;
	unsigned char* image_data = NULL;
	unsigned int image_data_len = 0;
	bool ret = true;
	try
	{
		readjpegfile(m_jpegfilename.c_str(), &image_data, &image_data_len);
		getexifblock(&exif_data, &exif_data_len);
		writejpegfile_with_exif(m_jpegfilename.c_str(), exif_data, exif_data_len, image_data, image_data_len);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "EXIF", e.what());
		ret =  false;
	}
	SAFE_FREE(exif_data);
	SAFE_ARRAY_DELETE(image_data);
	return ret;
}

void EXIFCreator::getexifblock(unsigned char** exif_data, unsigned int* length)
{
	exif_data_save_data(m_data, exif_data, length);
	if(exif_data == NULL)
		throw std::runtime_error("fail to exif_data_save_data");
}

long EXIFCreator::get_file_length(FILE* fp)
{
	long cur_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, cur_pos, SEEK_SET);
	return file_length;
}

int EXIFCreator::read_data_from_to(FILE* fp, long from, long to, unsigned char** buffer)
{
	int length = to - from+1;
	*buffer = new unsigned char[length];
	if(fseek(fp, from, SEEK_SET)!=0)
	{
		SAFE_ARRAY_DELETE(*buffer);
		return 0;
	}
	if(fread(*buffer, 1, length, fp) != length)
	{
		SAFE_ARRAY_DELETE(*buffer);
		return 0;
	}
	return length;
}

void EXIFCreator::readjpegfile(const char* jpegfilename, unsigned char** jpeg_data, unsigned int* length)
{
	const int image_start_offset = 20; //from SOI (FF D8)!! offset is 20 generally
	*jpeg_data = NULL;
	FILE* fp = fopen(jpegfilename, "rb");
	if(fp == NULL)
	{
		throw std::runtime_error("fail to open a file for reading");
	}
	int file_length = get_file_length(fp);
	*length = read_data_from_to(fp, image_start_offset, file_length-1, jpeg_data);
	fclose(fp);
	if(*length == 0)
	{
		throw std::runtime_error("fail to readjpegfile");
	}
}

void EXIFCreator::writejpegfile_with_exif(const char* dstjpegfilename, const unsigned char* exif_data, int exif_data_length, const unsigned char* image_data, int image_data_length)
{
	const char exif_header[] = { //exif
	  0xff, 0xd8, 0xff, 0xe1
	};
	std::ofstream fout;
	try
	{
		fout.open(dstjpegfilename, std::ios::binary);
		fout.write(exif_header, sizeof(exif_header));
		//big-endian according to exif spec
		fout.put((exif_data_length+2) >> 8);
		fout.put((exif_data_length+2) & 0xff);
		fout.write((const char*)exif_data, exif_data_length);
		fout.write((const char*)image_data, image_data_length);
		fout.close();
	}
	catch(std::exception& e)
	{
		if(fout.is_open())
			fout.close();
		throw std::runtime_error("fail to writejpegfile_with_exif");
	}
}



ExifData* EXIFCreator::create_exifdata()
{
	ExifData *data = exif_data_new();
	if(data == NULL)
	{
		dlog_print(DLOG_DEBUG, "EXIFCreator", "fail to exif_data_new");
		return NULL;
	}
	exif_data_set_option(data, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
	exif_data_set_data_type(data, EXIF_DATA_TYPE_COMPRESSED);
	exif_data_set_byte_order(data, EXIF_BYTE_ORDER_INTEL); //little endian
	exif_data_fix(data);

	return data;
}

ExifEntry* EXIFCreator::get_or_create_tag_with_memory_ifnone(ExifData* data, ExifIfd ifd, ExifTag tag, unsigned buflength)
{
	ExifEntry *entry = exif_content_get_entry(data->ifd[ifd], tag);
	if(entry == NULL) //create one
	{
		ExifMem *mem = exif_mem_new_default();
		entry = exif_entry_new_mem(mem);
		entry->data = (unsigned char*)exif_mem_alloc(mem, buflength);
		entry->size = (size_t)buflength;
		entry->tag = tag;
		entry->components = (size_t)buflength;
		entry->format = EXIF_FORMAT_UNDEFINED;
		exif_content_add_entry (data->ifd[ifd], entry);
		exif_mem_unref(mem);
		exif_entry_unref(entry);
	}
	return entry;
}

ExifEntry* EXIFCreator::get_or_create_tag_ifnone(ExifData* data, ExifIfd ifd, ExifTag tag)
{
	ExifEntry *entry = exif_content_get_entry(data->ifd[ifd], tag);
	if(entry == NULL) //create one
	{
		entry = exif_entry_new ();
		entry->tag = tag;
		exif_content_add_entry (data->ifd[ifd], entry);
		exif_entry_initialize (entry, tag);
		exif_entry_unref(entry);
	}
	return entry;
}
void EXIFCreator::setdata(unsigned char* dest, const char* text, int length)
{
	memcpy(dest, (char*)text, length);
}

void EXIFCreator::setdata(unsigned char* dest, ExifFormat format, int value)
{
	switch(format)
	{
		case EXIF_FORMAT_BYTE:
		{
			dest[0] = (ExifByte)value;
			break;
		}
		case EXIF_FORMAT_SHORT:
		{
			ExifShort var = (ExifShort)value;
			exif_set_short(dest, EXIF_BYTE_ORDER_INTEL, var);
			break;
		}
		case EXIF_FORMAT_LONG:
		{
			ExifLong var = (ExifLong)value;
			exif_set_long(dest, EXIF_BYTE_ORDER_INTEL, var);
			break;
		}

		case EXIF_FORMAT_SBYTE:
		{
			ExifSByte* sdest = (ExifSByte*)dest;
			sdest[0] = (ExifSByte)value;
			break;
		}
		case EXIF_FORMAT_SSHORT:
		{
			ExifSShort var = (ExifSShort)value;
			exif_set_sshort(dest, EXIF_BYTE_ORDER_INTEL, var);
			break;
		}
		case EXIF_FORMAT_SLONG:
		{
			ExifSLong var = (ExifSLong)value;
			exif_set_slong(dest, EXIF_BYTE_ORDER_INTEL, var);
			break;
		}
		case EXIF_FORMAT_RATIONAL:

		case EXIF_FORMAT_SRATIONAL:

		case EXIF_FORMAT_FLOAT:

		case EXIF_FORMAT_DOUBLE:

		case EXIF_FORMAT_UNDEFINED:

		case EXIF_FORMAT_ASCII:

		default:
			dlog_print(DLOG_ERROR, "EXIFCreator", "do not support %d type in this version", (int)format);
			break;
	}
}

