/*
 * muxer.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "muxer.h"
#include "transcoder.h"


/*
typedef struct _MuxerClass
{
	mediamuxer_h muxer;
}MuxerClass;

typedef void (*ReadSampleCB)(void* data, int track_index, media_packet_h* inbuf);
*/
#define MAX_TRACK_FOR_MUXER 2
static mediamuxer_h s_muxer = NULL;
static bool s_trackindex[MAX_TRACK_FOR_MUXER] = {0, 0};
static int s_numtrack = 0;

bool CreateMuxer(const char* dstfilepath, mediamuxer_output_format_e format)
{
	int ret = MEDIAMUXER_ERROR_NONE;
	if(s_muxer != NULL)
		return false;
	ret = mediamuxer_create(&s_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_create:%d", ret);
		return false;
	}
	ret = mediamuxer_set_data_sink(s_muxer, dstfilepath, format);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_set_data_sink:%d", ret);
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
	if(s_muxer != NULL)
		return MEDIAMUXER_ERROR_INVALID_OPERATION;
	ret = mediamuxer_add_track(s_muxer, media_format, &index);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_add_track:%d", ret);
	}
	s_trackindex[s_numtrack++] = index;
	return index;
}

bool StartMuxer(ReadSampleCB readsamplecb, void* data)
{

	int ret = MEDIAMUXER_ERROR_NONE;
	if(s_muxer != NULL)
		return false;
	ret = mediamuxer_prepare(s_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_prepare:%d", ret);
		return false;
	}
	ret = mediamuxer_start(s_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		mediamuxer_unprepare(s_muxer);
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_start:%d", ret);
		return false;
	}

	for(int i =0; i < s_numtrack; ++i)
	{
		media_packet_h inbuf = NULL;
		bool eos = false;
		while(eos == false)
		{
			readsamplecb(data, s_trackindex[i], &inbuf, &eos);
			ret = mediamuxer_write_sample(s_muxer, s_trackindex[i], inbuf);
			if(ret != MEDIAMUXER_ERROR_NONE)
			{
				dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_prepare:%d", ret);
				mediamuxer_stop(s_muxer);
				mediamuxer_unprepare(s_muxer);
				return false;
			}
		}
		ret = mediamuxer_close_track(s_muxer, s_trackindex[i]);
		if(ret != MEDIAMUXER_ERROR_NONE)
		{
			dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_prepare:%d", ret);
			mediamuxer_stop(s_muxer);
			mediamuxer_unprepare(s_muxer);
			return false;
		}
	}
	mediamuxer_stop(s_muxer);
	mediamuxer_unprepare(s_muxer);
	return true;
}


