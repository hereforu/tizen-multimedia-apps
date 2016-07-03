/*
 * test.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */
#include "transcoder.h"
#include "mediademuxer.h"
#include "mediamuxer.h"
#include "media_codec.h"


/*
 * deadlock happened in the mediademuxer_unprepare function
 */



void muxer_only()
{
	char* wow = "/home/owner/wow.mp4";
	//to check the error value
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_OUT_OF_MEMORY:%d", MEDIAMUXER_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_PARAMETER:%d", MEDIAMUXER_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_OPERATION:%d", MEDIAMUXER_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_NOT_SUPPORTED:%d", MEDIAMUXER_ERROR_NOT_SUPPORTED);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_PERMISSION_DENIED:%d", MEDIAMUXER_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_STATE:%d", MEDIAMUXER_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_PATH:%d", MEDIAMUXER_ERROR_INVALID_PATH);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_RESOURCE_LIMIT:%d", MEDIAMUXER_ERROR_RESOURCE_LIMIT);

	mediamuxer_h muxer = NULL;
	int ret = mediamuxer_create(&muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_create:%d", ret);
//	char* wow = const_cast<char*>(dstfilepath);
	ret = mediamuxer_set_data_sink(muxer, wow, MEDIAMUXER_CONTAINER_FORMAT_MP4);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_set_data_sink (%s):%d ", wow, ret);

	media_format_h audio_fmt = NULL;
	media_format_create(&audio_fmt);
	ret = media_format_set_audio_mime(audio_fmt, MEDIA_FORMAT_AAC_LC);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_mime:%d", ret);
	ret = media_format_set_audio_channel(audio_fmt, 2);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_channel:%d", ret);
	ret = media_format_set_audio_samplerate(audio_fmt, 44100);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_samplerate:%d", ret);
	ret = media_format_set_audio_bit(audio_fmt, 16);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_bit:%d", ret);
	ret = media_format_set_audio_avg_bps(audio_fmt, 128000);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_avg_bps:%d", ret);
	media_format_set_audio_aac_type(audio_fmt, true);
	int audio_index = -1;
	ret = mediamuxer_add_track(muxer, audio_fmt, &audio_index);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_add_track with index(%d):%d", audio_index, ret);
	//video format creation
	media_format_h video_fmt = NULL;
	media_format_create(&video_fmt);
	ret = media_format_set_video_mime(video_fmt, MEDIA_FORMAT_H264_SP);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_mime:%d", ret);
	ret = media_format_set_video_width(video_fmt, 640);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_width:%d", ret);
	ret = media_format_set_video_height(video_fmt, 480);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_height:%d", ret);
	ret = media_format_set_video_avg_bps(video_fmt, 256000);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_avg_bps:%d", ret);
	ret = media_format_set_video_max_bps(video_fmt, 256000);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_max_bps:%d", ret);
	int video_index = -1;
	ret = mediamuxer_add_track(muxer, video_fmt, &video_index);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_add_track with index(%d):%d", video_index, ret);



	ret = mediamuxer_prepare(muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_prepare:%d", ret);
}

#if 0
typedef struct _TrackInfo
{
	media_format_type_e formattype;
	media_format_h fmt;
	std::vector<media_packet_h> packets;
}TrackInfo;
void muxing(std::vector<TrackInfo>& tracks, const char* dstfilepath)
{
	//to check the error value
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_OUT_OF_MEMORY:%d", MEDIAMUXER_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_PARAMETER:%d", MEDIAMUXER_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_OPERATION:%d", MEDIAMUXER_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_NOT_SUPPORTED:%d", MEDIAMUXER_ERROR_NOT_SUPPORTED);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_PERMISSION_DENIED:%d", MEDIAMUXER_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_STATE:%d", MEDIAMUXER_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_INVALID_PATH:%d", MEDIAMUXER_ERROR_INVALID_PATH);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIAMUXER_ERROR_RESOURCE_LIMIT:%d", MEDIAMUXER_ERROR_RESOURCE_LIMIT);

	mediamuxer_h muxer = NULL;
	int ret = mediamuxer_create(&muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_create:%d", ret);
	char* wow = const_cast<char*>(dstfilepath);
	ret = mediamuxer_set_data_sink(muxer, wow, MEDIAMUXER_CONTAINER_FORMAT_MP4);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_set_data_sink (%s):%d ", wow, ret);

	media_format_h audio_fmt;
	media_format_create(&audio_fmt);
	ret = media_format_set_audio_mime(audio_fmt, MEDIA_FORMAT_AAC);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_mime:%d", ret);
	ret = media_format_set_audio_channel(audio_fmt, 2);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_channel:%d", ret);
	ret = media_format_set_audio_samplerate(audio_fmt, 48000);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_samplerate:%d", ret);
	ret = media_format_set_audio_bit(audio_fmt, 16);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_bit:%d", ret);
	ret = media_format_set_audio_avg_bps(audio_fmt, 128000);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_audio_avg_bps:%d", ret);
	int audio_index = -1;
	ret = mediamuxer_add_track(muxer, audio_fmt, &audio_index);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_add_track with index(%d):%d", audio_index, ret);


	//video format creation
	media_format_h video_fmt;
	media_format_create(&video_fmt);
	ret = media_format_set_video_mime(video_fmt, MEDIA_FORMAT_H264_SP);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_mime:%d", ret);
	ret = media_format_set_video_width(video_fmt, 640);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_width:%d", ret);
	ret = media_format_set_video_height(video_fmt, 480);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_height:%d", ret);
	ret = media_format_set_video_avg_bps(video_fmt, 256000);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_avg_bps:%d", ret);
	ret = media_format_set_video_max_bps(video_fmt, 256000);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "media_format_set_video_max_bps:%d", ret);
	int video_index = -1;
	ret = mediamuxer_add_track(muxer, video_fmt, &video_index);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_add_track with index(%d):%d", video_index, ret);



	ret = mediamuxer_prepare(muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_prepare:%d", ret);

	/*
	ret = mediamuxer_start(muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_start:%d", ret);
	for(int i = 0; i < video_packets.size(); ++i)
	{
		ret = mediamuxer_write_sample(muxer, track_index, video_packets[i]);
		dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_write_ %d th sample:%d", i, ret);
	}
	ret = mediamuxer_close_track(muxer, track_index);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_close_track:%d", ret);
	ret = mediamuxer_stop(muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_stop:%d", ret);
	ret = mediamuxer_unprepare(muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_unprepare:%d", ret);
	ret = mediamuxer_destroy(muxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediamuxer_destroy:%d", ret);
	*/
}


media_format_type_e check_formatinfo(int track_index, media_format_h fmt)
{
	media_format_type_e formattype;
	int ret = media_format_get_type(fmt, &formattype);
	media_format_mimetype_e mimetype;
	int width = 0, height = 0, avg_bps=0, max_bps = 0;
	int channel=0, samplerate=0, bit=0;

	switch(formattype)
	{
		case MEDIA_FORMAT_AUDIO:
			dlog_print(DLOG_DEBUG, "TRANS_TEST", "===== %d th track is audio (%x)=====", track_index, formattype);
			ret = media_format_get_audio_info(fmt, &mimetype, &channel, &samplerate, &bit, &avg_bps);
			dlog_print(DLOG_DEBUG, "TRANS_TEST", "===== audio mimetype:%x, channel:%d, samplerate:%d, bit:%d, avg_bps:%d =====", (int)mimetype, channel, samplerate, bit, avg_bps);
			break;
		case MEDIA_FORMAT_VIDEO:
			dlog_print(DLOG_DEBUG, "TRANS_TEST", "===== %d th track is video (%x)=====", track_index, formattype);
			ret = media_format_get_video_info(fmt, &mimetype, &width, &height, &avg_bps, &max_bps);
			dlog_print(DLOG_DEBUG, "TRANS_TEST", "===== vidoe mimetype: %x, w:%d, h:%d, avg_bps:%d, max_bps:%d =====", (int)mimetype, width, height, avg_bps, max_bps);
			break;
		case MEDIA_FORMAT_CONTAINER:
			dlog_print(DLOG_DEBUG, "TRANS_TEST", "===== %d th track is container (%x)=====", track_index, formattype);
			ret = media_format_get_container_mime(fmt, &mimetype);
			dlog_print(DLOG_DEBUG, "TRANS_TEST", "===== container mimetype: %x =====", (int)mimetype);
			break;
		default:
			dlog_print(DLOG_DEBUG, "TRANS_TEST", "===== %d th track is ??? : formattype:(%x)=====", track_index, formattype);
			ret = media_format_get_video_info(fmt, &mimetype, &width, &height, &avg_bps, &max_bps);

			break;
	}
	return formattype;
}
bool g_exit = false;
void muxer_eos_cb(int track_num, void *user_data)
{
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "muxer_eos_cb track num:%d", track_num);
	g_exit = true;

}


void start_transcoding(const char* srcfilepath)
{
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_OUT_OF_MEMORY:%d", MEDIADEMUXER_ERROR_OUT_OF_MEMORY);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_INVALID_PARAMETER:%d", MEDIADEMUXER_ERROR_INVALID_PARAMETER);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_INVALID_OPERATION:%d", MEDIADEMUXER_ERROR_INVALID_OPERATION);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_NOT_SUPPORTED:%d", MEDIADEMUXER_ERROR_NOT_SUPPORTED);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_PERMISSION_DENIED:%d", MEDIADEMUXER_ERROR_PERMISSION_DENIED);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_INVALID_STATE:%d", MEDIADEMUXER_ERROR_INVALID_STATE);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_INVALID_PATH:%d", MEDIADEMUXER_ERROR_INVALID_PATH);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_RESOURCE_LIMIT:%d", MEDIADEMUXER_ERROR_RESOURCE_LIMIT);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_SEEK_FAILED:%d", MEDIADEMUXER_ERROR_SEEK_FAILED);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "MEDIADEMUXER_ERROR_DRM_NOT_PERMITTED:%d", MEDIADEMUXER_ERROR_DRM_NOT_PERMITTED);

	mediademuxer_h demuxer = NULL;
	int ret = mediademuxer_create(&demuxer);
	mediademuxer_set_eos_cb(demuxer, muxer_eos_cb,  NULL);
	ret = mediademuxer_set_data_source(demuxer, srcfilepath);
	ret = mediademuxer_prepare(demuxer);
	int count = 0;
	ret = mediademuxer_get_track_count(demuxer, &count);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "%s track count:%d", srcfilepath, count);





	std::vector<TrackInfo> tracks;
	for(int i = 0; i < count; ++i)
	{
		TrackInfo track;
		ret = mediademuxer_get_track_info(demuxer, i, &track.fmt);
		track.formattype = check_formatinfo(i, track.fmt);
		ret = mediademuxer_select_track(demuxer, i);
		ret = mediademuxer_start(demuxer);
		dlog_print(DLOG_DEBUG, "TRANS_TEST", "demuxer has been started with return value: %d", ret);
		int iter = 0;
		g_exit = false;
		while(g_exit == false)
		{
			media_packet_h packet;
			ret = mediademuxer_read_sample(demuxer, i, &packet);
			if(ret == MEDIADEMUXER_ERROR_NONE)
			{
				track.packets.push_back(packet);
			}
			else
			{
				dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediademuxer_read_sample error:%d", ret);
			}
			++iter;
			if(iter > 10000)
				break;
		}
		dlog_print(DLOG_DEBUG, "TRANS_TEST", "%d packets have been pushed", track.packets.size());
		ret = mediademuxer_unselect_track(demuxer, i);
		dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediademuxer_unselect_track:%d", ret);
		ret = mediademuxer_stop(demuxer);
		dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediademuxer_stop:%d", ret);
		tracks.push_back(track);
	}
	ret = mediademuxer_unprepare(demuxer); //it blocks always
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediademuxer_unprepare:%d", ret);
	ret = mediademuxer_unset_eos_cb(demuxer); //mediademuxer_unset_eos_cb gives the MEDIADEMUXER_ERROR_INVALID_STATE but there is no comment about that
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediademuxer_unset_eos_cb:%d", ret);
	ret = mediademuxer_destroy(demuxer);
	dlog_print(DLOG_DEBUG, "TRANS_TEST", "mediademuxer_destroy:%d", ret);

	std::string dstfilepath = "/home/owner/content/Camera/wow.mp4";


	muxing(tracks, dstfilepath.c_str());
}

#endif


