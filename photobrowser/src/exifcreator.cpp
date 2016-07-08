/*
 * exifcreator.cpp
 *
 *  Created on: Jul 6, 2016
 *      Author: Jason
 */

#include "base.h"
#include "exifcreator.h"
#include <stdexcept>

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
void EXIFCreator::WriteExif()
{
	write_exif_to_file(m_jpegfilename.c_str(), m_jpegfilename.c_str());
}

void EXIFCreator::getexifblock(unsigned char** exif_data, unsigned int* length)
{
	exif_data_save_data(m_data, exif_data, length);
	if(exif_data == NULL)
		throw std::runtime_error("fail to exif_data_save_data");
}

void EXIFCreator::readjpegfile(const char* jpegfilename, unsigned char** jpeg_data, unsigned int* length)
{
	FILE* fp = fopen(jpegfilename, "rb");
	if(fp == NULL)
	{
		throw std::runtime_error("fail to open a file for reading");
	}
	if(fseek(fp, 0, SEEK_END) != 0)
	{
		fclose(fp);
		throw std::runtime_error("fail to fseek to the end ");
	}
	const int image_start_offset = 20; //from FF DB!! offset is 20 generally
	*length = ftell(fp)-image_start_offset;
	if(fseek(fp, image_start_offset, SEEK_SET) != 0)
	{
		fclose(fp);
		throw std::runtime_error("fail to fseek to the first of the file");
	}
	*jpeg_data = (unsigned char*)malloc(*length);
	if(*jpeg_data == NULL)
	{
		fclose(fp);
		throw std::runtime_error("out of memory to make the buffer");
	}
	if(fread(*jpeg_data, 1, *length, fp) != *length)
	{
		fclose(fp);
		throw std::runtime_error("fail to read the content");
	}
	fclose(fp);
}

void EXIFCreator::writejpegfile_with_exif(const char* dstjpegfilename, unsigned char* exif_data, int exif_data_length, unsigned char* image_data, int image_data_length)
{
	const unsigned char exif_header[] = {
	  0xff, 0xd8, 0xff, 0xe1
	}; //exif

	FILE* fp = fopen(dstjpegfilename, "wb");
	if(fp == NULL)
	{
		throw std::runtime_error("fail to open file for writing");
	}
	if(fwrite(exif_header, 4, 1, fp) != 1)
	{
		fclose(fp);
		throw std::runtime_error("fail to write exif header");
	}
	//big-endian according to exif spec
	if (fputc((exif_data_length+2) >> 8, fp) < 0)
	{
		fclose(fp);
		throw std::runtime_error("fail to write exif_data_length");
	}
	if (fputc((exif_data_length+2) & 0xff, fp) < 0)
	{
		fclose(fp);
		throw std::runtime_error("fail to write exif_data_length");
	}
	if(fwrite(exif_data, exif_data_length, 1, fp) != 1)
	{
		fclose(fp);
		throw std::runtime_error("fail to write exif data");
	}

	if(fwrite(image_data, image_data_length, 1, fp) != 1)
	{
		fclose(fp);
		throw std::runtime_error("fail to write image data");
	}
}

bool EXIFCreator::write_exif_to_file(const char* srcjpegfilename, const char* destjpegfilename)
{
	unsigned char* exif_data = NULL;
	unsigned int exif_data_len = 0;
	unsigned char* image_data = NULL;
	unsigned int image_data_len = 0;

	bool ret = true;
	try
	{
		getexifblock(&exif_data, &exif_data_len);
		readjpegfile(srcjpegfilename, &image_data, &image_data_len);
		writejpegfile_with_exif(destjpegfilename, exif_data, exif_data_len, image_data, image_data_len);
	}
	catch(const std::runtime_error& e)
	{

		dlog_print(DLOG_ERROR, "EXIF", e.what());
		ret =  false;
	}
	if(exif_data)
	{
		free(exif_data);
		exif_data = NULL;
	}
	if(image_data)
	{
		free(image_data);
		image_data = NULL;
	}

	return ret;
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
	memcpy(dest, (unsigned char*)text, length);
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

