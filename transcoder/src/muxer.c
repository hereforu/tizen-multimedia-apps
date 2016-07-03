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
static bool s_trackindex[MAX_TRACK_FOR_MUXER] = {0, 0};
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
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_create:%d", ret);
		return false;
	}
	ret = mediamuxer_set_data_sink(s_muxer, path, format);
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
#if 0
	int ret = MEDIAMUXER_ERROR_NONE;
	int index = MEDIAMUXER_ERROR_INVALID_PARAMETER;
	if(s_muxer == NULL)
		return MEDIAMUXER_ERROR_INVALID_OPERATION;
	ret = mediamuxer_add_track(s_muxer, media_format, &index);
	if(ret != MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_add_track:%d", ret);
	}
	s_trackindex[s_numtrack++] = index;
	return index;
#endif
	int index = -1;
	media_format_h video_fmt = NULL;
	media_format_create(&video_fmt);
	media_format_set_video_mime(video_fmt, MEDIA_FORMAT_H264_SP);
	media_format_set_video_width(video_fmt, 640);
	media_format_set_video_height(video_fmt, 480);
	media_format_set_video_avg_bps(video_fmt, 256000);
	media_format_set_video_max_bps(video_fmt, 256000);
	int ret = mediamuxer_add_track(s_muxer, video_fmt, &index);
	dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_add_track with video index %d:%d", index, ret);
	s_trackindex[s_numtrack++] = index;

	media_format_h audio_fmt = NULL;
	media_format_create(&audio_fmt);
	media_format_set_audio_mime(audio_fmt, MEDIA_FORMAT_AAC_LC);
	media_format_set_audio_channel(audio_fmt, 2);
	media_format_set_audio_samplerate(audio_fmt, 44100);
	media_format_set_audio_bit(audio_fmt, 16);
	media_format_set_audio_avg_bps(audio_fmt, 128000);
	ret = mediamuxer_add_track(s_muxer, audio_fmt, &index);
	dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_add_track with audio index %d:%d", index, ret);
	s_trackindex[s_numtrack++] = index;

	return 0;

}

bool StartMuxer(ReadSampleCB readsamplecb, void* data)
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
		mediamuxer_unprepare(s_muxer);
		dlog_print(DLOG_ERROR, "MUXER", "mediamuxer_start:%d", ret);
		return false;
	}
	dlog_print(DLOG_DEBUG, "MUXER", "mediamuxer_start success", ret);

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


