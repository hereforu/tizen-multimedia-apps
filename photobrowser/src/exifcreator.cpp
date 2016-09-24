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
	Destroy();
}

void EXIFCreator::Create(const char* jpegfilename)
{
	m_data = create_exifdata();
	m_jpegfilename = jpegfilename;
}


void EXIFCreator::Destroy()
{
	if(m_data)
	{
		exif_data_unref(m_data);
		m_data = NULL;
	}
}

/*
 * When adding a value that corresponds to a tag in ExifData,
 * the tag first has to be created and value assigned to it.
 * The function AddResolution creates tags for ImageWidth,
 * ImageLength and writes the proper values for each
 */
void EXIFCreator::AddResolution(int width, int height)
{
	ExifEntry* width_entry = get_or_create_tag_ifnone(m_data, EXIF_IFD_0, EXIF_TAG_IMAGE_WIDTH);
	setdata(width_entry->data, width_entry->format, width);
	ExifEntry* length_entry = get_or_create_tag_ifnone(m_data, EXIF_IFD_0, EXIF_TAG_IMAGE_LENGTH);
	setdata(length_entry->data, length_entry->format, height);
}

/*
 * When inserting ASCII string such as adding comment,
 * then memory has to be allocated for entry->data.
 * The function get_or_create_tag_with_memory_ifnone can be called
 * if the tag under consideration requires memory.
 * To specify value, the string array can be copied directly in memory to entry->data
 */
void EXIFCreator::AddComment(const char* text)
{
	int len = strlen(text);
	ExifEntry* comment_entry = get_or_create_tag_with_memory_ifnone(m_data, EXIF_IFD_EXIF, EXIF_TAG_USER_COMMENT, 8+len);
	char* mem = new char[8+len];
	memcpy(mem, "ASCII\0\0\0", 8);
	memcpy(mem+8, text, len);
	setdata(comment_entry->data, mem, 8+len);
	delete[] mem;
}

/*
 * WriteExif constructs exif block and merges the two data blocks in the image
 */
void EXIFCreator::WriteExif()
{
	unsigned char* exif_data = NULL;
	unsigned int exif_data_len = 0;
	unsigned char* image_data = NULL;
	unsigned int image_data_len = 0;
	try
	{
		readjpegfile(m_jpegfilename.c_str(), &image_data, &image_data_len);
		getexifblock(&exif_data, &exif_data_len);
		writejpegfile_with_exif(m_jpegfilename.c_str(), exif_data, exif_data_len, image_data, image_data_len);
	}
	catch(const std::runtime_error& e)
	{
		SAFE_FREE(exif_data);
		SAFE_ARRAY_DELETE(image_data);
		throw e;
	}
	SAFE_FREE(exif_data);
	SAFE_ARRAY_DELETE(image_data);
}

/*
 * getexifblock constructs data chunk, which can be readily added to the JPEG file via exif_data_save_data function of libExif
 */
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

/*
 * readjpegfile opens the specified JPEG file and reads from Start of Image(SOI).
 * The content is stored injpeg_data and passed to the caller of the function.
 */
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

/*
 * exif heaer, exif data length, exif data, and the JPEG data from the source file
 * are written in sequence in order to write as the resultant file.
 */
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
	catch(const std::exception& e)
	{
		if(fout.is_open())
			fout.close();
		throw std::runtime_error("fail to writejpegfile_with_exif");
	}
}

/*
 * create_exifdata creates ExifData by using many of the functions provided by libExif
 * and makes specifications for a few options.
 * In the sample application, the JPEG image compression is set to 'comply' and EXIF block byte order to little endian
 */

ExifData* EXIFCreator::create_exifdata()
{
	ExifData *data = exif_data_new();
	if(data == NULL)
	{
		throw std::runtime_error("fail to exif_data_new");
	}
	exif_data_set_option(data, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
	exif_data_set_data_type(data, EXIF_DATA_TYPE_COMPRESSED);
	exif_data_set_byte_order(data, EXIF_BYTE_ORDER_INTEL); //little endian
	exif_data_fix(data);
	return data;
}

/* This is needed when exif_entry_initialize() isn't able to create
 * this type of tag itself, or the default data length it creates isn't the
 * correct length.
 */
ExifEntry* EXIFCreator::get_or_create_tag_with_memory_ifnone(ExifData* data, ExifIfd ifd, ExifTag tag, unsigned buflength)
{
	ExifEntry *entry = exif_content_get_entry(data->ifd[ifd], tag);
	if(entry == NULL) //create one
	{
		 /* Create a memory allocator to manage this ExifEntry */
		ExifMem *mem = exif_mem_new_default();
		if(mem == NULL)
			throw std::runtime_error("fail to exif_mem_new_default");
		/* Create a new ExifEntry using our allocator */
		entry = exif_entry_new_mem(mem);
		if(entry == NULL)
		{
			exif_mem_unref(mem);
			throw std::runtime_error("fail to exif_entry_new_mem");
		}
		entry->data = (unsigned char*)exif_mem_alloc(mem, buflength);
		entry->size = (size_t)buflength;
		entry->tag = tag;
		entry->components = (size_t)buflength;
		entry->format = EXIF_FORMAT_UNDEFINED;
		 /* Attach the ExifEntry to an IFD */
		exif_content_add_entry (data->ifd[ifd], entry);
		exif_mem_unref(mem);
		exif_entry_unref(entry);
	}
	return entry;
}

/*
 * It fetches a specific tag from ExifData or adds a specified tag to image.
 */
ExifEntry* EXIFCreator::get_or_create_tag_ifnone(ExifData* data, ExifIfd ifd, ExifTag tag)
{
	ExifEntry *entry = exif_content_get_entry(data->ifd[ifd], tag);
	if(entry == NULL) //create one
	{
		/* Allocate a new entry */
		entry = exif_entry_new ();
		if(entry == NULL)
			throw std::runtime_error("fail to exif_entry_new");
		entry->tag = tag;
		/* Attach the ExifEntry to an IFD */
		exif_content_add_entry (data->ifd[ifd], entry);
		 /* Allocate memory for the entry and fill with default data */
		exif_entry_initialize (entry, tag);
		/* Ownership of the ExifEntry has now been passed to the IFD.
		 * One must be very careful in accessing a structure after
		 * unref'ing it; in this case, we know "entry" won't be freed
		 * because the reference count was bumped when it was added to
		 * the IFD.
		 */
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

