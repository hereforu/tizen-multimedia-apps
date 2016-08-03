/*
 * muxer.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "muxer.h"
#include "transcoder.h"
#include <string.h>


#define MAX_TRACK_FOR_MUXER 2

static mediamuxer_h s_muxer = NULL;
static int s_numtrack = 0;

bool CreateMuxer(const char* dstfilepath, mediamuxer_output_format_e format)
{
	char path[256]={0, };
	int ret = MEDIAMUXER_ERROR_NONE;
	strcpy(path, dstfilepath);
	if(s_muxer != NULL)
		return false;
	ret = mediamuxer_create(&s_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "MUXER", "mediamuxer_create:%d", ret);
		return false;
	}
	dlog_print(DLOG_ERROR, "MUXER", "mediamuxer_set_data_sink param:%s, format:%d", path, (int)format);
	ret = mediamuxer_set_data_sink(s_muxer, const_cast<char*>(dstfilepath), format);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "MUXER", "mediamuxer_set_data_sink:%d", ret);
		return false;
	}
	s_numtrack = 0;
	return true;
}

void DestroyMuxer()
{
	int ret = MEDIAMUXER_ERROR_NONE;
	if(s_muxer == NULL)
		return;
	ret = mediamuxer_destroy(s_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_destroy:%d", ret);
		return;
	}
}

int AddTrack(media_format_h media_format)
{

	int ret = MEDIAMUXER_ERROR_NONE;
	int index = MEDIAMUXER_ERROR_INVALID_PARAMETER;
	if(s_muxer == NULL)
		return MEDIAMUXER_ERROR_INVALID_OPERATION;
	ret = mediamuxer_add_track(s_muxer, media_format, &index);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_add_track:%d", ret);
	}
	return index;
}

bool StartMuxer()
{
	int ret = MEDIAMUXER_ERROR_NONE;
	if(s_muxer == NULL)
		return false;
	ret = mediamuxer_prepare(s_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "MUXER", "mediamuxer_prepare:%d", ret);
		return false;
	}
	dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_prepare success", ret);
	ret = mediamuxer_start(s_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "MUXER", "mediamuxer_start:%d before unprepare", ret);
		mediamuxer_unprepare(s_muxer);
		dlog_print(DLOG_ERROR, "MUXER", "mediamuxer_start:%d after unprepare", ret);
		return false;
	}
	dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_start success", ret);
	return true;
}
bool WriteSample(int track_index, media_packet_h sample)
{
	int ret = mediamuxer_write_sample(s_muxer, track_index, sample);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "fail to mediamuxer_write_sample:%d", ret);
		return false;
	}
	return true;
}
bool CloseTrack(int track_index)
{
	int ret = mediamuxer_close_track(s_muxer, track_index);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "fail to close track:%d", ret);
		return false;
	}
	return true;
}
bool StopMuxer()
{
	if(s_muxer == NULL)
		return false;
	mediamuxer_stop(s_muxer);
	mediamuxer_unprepare(s_muxer);
	return true;
}




