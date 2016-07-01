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
