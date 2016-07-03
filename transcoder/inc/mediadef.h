/*
 * mediadef.h
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#ifndef MEDIADEF_H_
#define MEDIADEF_H_


#include "mediademuxer.h"
#include "mediamuxer.h"
#include "media_codec.h"
#include "sharedqueue.h"



typedef struct _VDecInfo
{
	mediacodec_codec_type_e codecid;
	int width;
	int height;
}VDecInfo;

typedef struct _VEncInfo
{
	mediacodec_codec_type_e codecid;
	int width;
	int height;
	int fps;
	int target_bits;
}VEncInfo;

typedef struct _ADecInfo
{
	mediacodec_codec_type_e codecid;
	int samplerate;
	int channel;
	int bit;
}ADecInfo;

typedef struct _AEncInfo
{
	mediacodec_codec_type_e codecid;
	int samplerate;
	int channel;
	int bit;
	int bitrate;
}AEncInfo;

typedef union _CodecInfo
{
	VDecInfo vdec;
	VEncInfo venc;
	ADecInfo adec;
	AEncInfo aenc;
}CodecInfo;

typedef struct _TrackInfo
{
	_TrackInfo()
	:queue(NULL)
	{

	}
	media_format_h fmt;
	SharedQueue* queue;
}TrackInfo;


#endif /* MEDIADEF_H_ */
