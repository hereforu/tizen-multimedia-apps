/*
 * exifcreator.h
 *
 *  Created on: Jul 6, 2016
 *      Author: Jason
 */

#ifndef EXIFCREATOR_H_
#define EXIFCREATOR_H_

#include <string>
#include <vector>
#include <map>
#include <libexif/exif-data.h>


class EXIFCreator
{
public:
	EXIFCreator();
	~EXIFCreator();

	bool Create(const char* jpegfilename);
	bool IsCreated();
	void Destroy();
	void AddResolution(int width, int height);
	void AddComment(const char* text);
	bool WriteExif();

private:
	void getexifblock(unsigned char** exif_data, unsigned int* length);
	void readjpegfile(const char* jpegfilename, unsigned char** jpeg_data, unsigned int* length);
	void writejpegfile_with_exif(const char* dstjpegfilename, const unsigned char* exif_data, int exif_data_length, const unsigned char* image_data, int image_data_length);
	ExifData* create_exifdata();
	ExifEntry* get_or_create_tag_with_memory_ifnone(ExifData* data, ExifIfd ifd, ExifTag tag, unsigned buflength);
	ExifEntry* get_or_create_tag_ifnone(ExifData* data, ExifIfd ifd, ExifTag tag);
	void setdata(unsigned char* dest, ExifFormat format, int value);
	void setdata(unsigned char* dest, const char* text, int length);
	long get_file_length(FILE* fp);
	int read_data_from_to(FILE* fp, long from, long to, unsigned char** buffer);

private:
	ExifData* m_data;
	std::string m_jpegfilename;
};


//refer https://github.com/wang-bin/libexif-port/blob/master/contrib/examples/write-exif.c
#endif /* EXIFCREATOR_H_ */
