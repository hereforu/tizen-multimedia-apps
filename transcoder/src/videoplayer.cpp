/*
 * videoplayer.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */


#include "common/base.h"
#include "videoplayer.h"
#include <stdexcept>

VideoPlayer::VideoPlayer()
:m_player(NULL)
{

}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::Create()
{
	int ret = PLAYER_ERROR_NONE;
	if((ret = player_create(&m_player))!= PLAYER_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to create player")+to_string(ret));
	}

}

void VideoPlayer::Destroy()
{
	if(m_player != NULL)
		player_destroy(m_player);
}
void VideoPlayer::Play()
{
	player_start(m_player);
}
void VideoPlayer::Stop()
{
	player_stop(m_player);
}
void VideoPlayer::SetMediaAndPrepare(const char* filepath, Evas_Object* display)
{
	int ret = PLAYER_ERROR_NONE;
	if((ret = player_set_uri(m_player, filepath))!= PLAYER_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to set ") + filepath + to_string(ret));
	}
	dlog_print(DLOG_DEBUG, "VideoPlayer", "player_set_uri(%s)[%d]", filepath, ret);
	if(display)
	{
		if((ret = player_set_display(m_player, PLAYER_DISPLAY_TYPE_EVAS, GET_DISPLAY(display)))!= PLAYER_ERROR_NONE)
		{
			dlog_print(DLOG_ERROR, "VideoPlayer", "fail to player_set_display[%d]", ret);
			throw std::runtime_error(std::string("fail to player_set_display")+to_string(ret));
		}

		dlog_print(DLOG_DEBUG, "VideoPlayer", "player_set_display[%d]", ret);
	//	ret = player_set_display_mode(m_player, PLAYER_DISPLAY_MODE_FULL_SCREEN);
	//	dlog_print(DLOG_DEBUG, "VideoPlayer", "player_set_display_mode[%d]", ret);
	//	ret = player_set_display_visible(m_player, true);
	//	dlog_print(DLOG_DEBUG, "VideoPlayer", "player_set_display_visible[%d]", ret);

	}
	if((ret = player_prepare(m_player))!= PLAYER_ERROR_NONE)
	{
		throw std::runtime_error(std::string("fail to prepare ") + filepath + to_string(ret));
	}
	dlog_print(DLOG_DEBUG, "VideoPlayer", "player_prepare [%d]", ret);
}

VideoInfo VideoPlayer::GetVideoCodecInfo()
{
	char* vcodec = NULL;
	char* acodec = NULL;
	if(player_get_codec_info(m_player, &acodec, &vcodec)!= PLAYER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "VideoPlayer", "fail to player_get_codec_info");
	}
	else
	{
		if(acodec)
		{
			m_videoinfo.audio_codec = acodec;
			free(acodec);
		}
		if(vcodec)
		{
			m_videoinfo.video_codec = vcodec;
			free(vcodec);
		}
	}
	if(player_get_video_stream_info(m_player, &m_videoinfo.fps, &m_videoinfo.video_bitrate)!= PLAYER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "VideoPlayer", "fail to player_get_video_stream_info");
	}
	return m_videoinfo;
}

