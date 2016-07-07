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
	void WriteExif();

private:
	void getexifblock(unsigned char** exif_data, unsigned int* length);
	void readjpegfile(const char* jpegfilename, unsigned char** jpeg_data, unsigned int* length);
	void writejpegfile_with_exif(const char* dstjpegfilename, unsigned char* exif_data, int exif_data_length, unsigned char* image_data, int image_data_length);
	bool write_exif_to_file(const char* srcjpegfilename, const char* destjpegfilename);
	ExifData* create_exifdata();
	ExifEntry* get_or_create_tag_ifnone(ExifData* data, ExifIfd ifd, ExifTag tag);
	void setdata(unsigned char* dest, ExifFormat format, int value);

private:
	ExifData* m_data;
	std::string m_jpegfilename;
};


//refer https://github.com/wang-bin/libexif-port/blob/master/contrib/examples/write-exif.c
#endif /* EXIFCREATOR_H_ */
