/*
 * demuxer.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "base.h"
#include "demuxer.h"
#include <stdexcept>


Demuxer::Demuxer()
:m_videotrackindex(-1), m_audiotrackindex(-1),m_demuxer(NULL)
{

}
Demuxer::~Demuxer()
{

}

void Demuxer::Create(const char* srcfilename)
{

	iferror_throw(mediademuxer_create(&m_demuxer), "fail to create demuxer: ");
	iferror_throw(mediademuxer_set_eos_cb(m_demuxer, demuxer_eos_cb, (void*)this), "fail to mediademuxer_set_eos_cb: ");
	iferror_throw(mediademuxer_set_error_cb(m_demuxer, demuxer_error_cb, (void*)this), "fail to mediademuxer_set_error_cb: ");
	iferror_throw(mediademuxer_set_data_source(m_demuxer, srcfilename), "fail to mediademuxer_set_data_source: ");
	iferror_throw(mediademuxer_prepare(m_demuxer), "fail to mediademuxer_prepare: ");
	extract_tracks();
}
void Demuxer::Destroy()
{
	for(int i= 0; i < m_tracks.size(); ++i)
	{
		media_format_unref(m_tracks[i].fmt);
	}
	m_tracks.clear();
	mediademuxer_unset_eos_cb(m_demuxer);
	mediademuxer_unset_error_cb(m_demuxer);
	mediademuxer_unprepare(m_demuxer);
	mediademuxer_destroy(m_demuxer);
	m_demuxer = NULL;

}
void Demuxer::Start()
{
	try
	{
		for(int i= 0; i < m_tracks.size(); ++i)
		{
			demuxing(i);
		}
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "Demuxer", e.what());
	}
}

unsigned int Demuxer::GetNumTracks()
{
	return m_tracks.size();
}
media_format_h Demuxer::GetMediaFormat(int track_index)
{
	return m_tracks[track_index].fmt;
}
bool Demuxer::GetVideoDecInfo(CodecInfo& vdec)
{
	if(m_videotrackindex == -1)
		return false;
	media_format_mimetype_e mimetype;
	int width, height, avg_bps, max_bps;
	int ret = media_format_get_video_info(m_tracks[m_videotrackindex].fmt, &mimetype, &width, &height, &avg_bps, &max_bps);
	if(ret == MEDIA_FORMAT_ERROR_NONE)
	{
		vdec.vdec.codecid = (mediacodec_codec_type_e) (mimetype & 0x0000FFFF);
		vdec.vdec.width = width;
		vdec.vdec.height = height;
		return true;
	}
	else
	{
		dlog_print(DLOG_ERROR, "Demuxer", "get video track info (%d)", ret);
	}

	return false;
}
bool Demuxer::GetAudioDecInfo(CodecInfo& adec)
{
	if(m_audiotrackindex == -1)
		return false;
	media_format_mimetype_e mimetype;
	int channel, samplerate, bit, avg_bps;
	int ret = media_format_get_audio_info(m_tracks[m_audiotrackindex].fmt, &mimetype, &channel, &samplerate, &bit, &avg_bps);
	if(ret == MEDIA_FORMAT_ERROR_NONE)
	{
		adec.adec.codecid = (mediacodec_codec_type_e)(mimetype & 0x0000FFFF);
		adec.adec.samplerate = samplerate;
		adec.adec.channel = channel;
		adec.adec.bit = bit;
		return true;
	}
	else
	{
		dlog_print(DLOG_ERROR, "Demuxer", "get audio track info (%d)", ret);
	}
	return false;
}

bool Demuxer::SetVideoQueue(SharedQueue* queue)
{
	if(m_videotrackindex == -1)
		return false;
	m_tracks[m_videotrackindex].queue = queue;
	return true;
}

bool Demuxer::SetAudioQueue(SharedQueue* queue)
{
	if(m_audiotrackindex == -1)
		return false;
	m_tracks[m_audiotrackindex].queue = queue;
	return true;
}

void Demuxer::demuxing(int track_index)
{
	dlog_print(DLOG_DEBUG, "Demuxer", "start of read of %d track", track_index);
	m_eosflag = false;
	AppTool::Assert(m_tracks[track_index].queue);
	iferror_throw(mediademuxer_select_track(m_demuxer, track_index), "fail to mediademuxer_select_track: ");
	iferror_throw(mediademuxer_start(m_demuxer), "fail to mediademuxer_start: ");
	int numsample = 0;
	while(m_eosflag == false)
	{
		media_packet_h packet = NULL;
		iferror_throw(mediademuxer_read_sample(m_demuxer, track_index, &packet), "fail to mediademuxer_read_sample: ");
		while(m_tracks[track_index].queue->IsFull())
			sleep(1);
		m_tracks[track_index].queue->Push(packet);
		++numsample;
	}
	m_tracks[track_index].queue->PushEoS();
	dlog_print(DLOG_DEBUG, "Demuxer", "end of read %d samples", numsample);
	iferror_throw(mediademuxer_stop(m_demuxer), "fail to mediademuxer_stop: ");
	iferror_throw(mediademuxer_unselect_track(m_demuxer, track_index), "fail to mediademuxer_unselect_track: ");
}

void Demuxer::extract_tracks()
{
	int trackcount = 0;
	iferror_throw(mediademuxer_get_track_count(m_demuxer, &trackcount), "fail to mediademuxer_get_track_count: ");
	for(int i= 0; i < trackcount; ++i)
	{
		TrackInfo track;
		iferror_throw(mediademuxer_get_track_info(m_demuxer, i, &track.fmt), "fail to mediademuxer_get_track_info: ");
		media_format_type_e formattype;
		int ret = media_format_get_type(track.fmt, &formattype);
		if(ret == MEDIA_FORMAT_ERROR_NONE)
		{
			if(formattype == MEDIA_FORMAT_VIDEO)
				m_videotrackindex = i;
			else if(formattype == MEDIA_FORMAT_AUDIO)
				m_audiotrackindex = i;
			m_tracks.push_back(track);
		}
		else
		{
			dlog_print(DLOG_ERROR, "Demuxer", "fail to media_format_get_type (%d)", ret);
		}
	}
}
void Demuxer::iferror_throw(int ret, const char* msg)
{
	if(ret != MEDIADEMUXER_ERROR_NONE)
		throw std::runtime_error(std::string(msg)+AppTool::ToString(ret));
}

void Demuxer::handle_eos(int track_num)
{
	dlog_print(DLOG_DEBUG, "Demuxer", "eos for %d track", track_num);
	m_eosflag = true;
}
void Demuxer::handle_error(mediademuxer_error_e error)
{

}

void Demuxer::demuxer_eos_cb(int track_num, void *user_data)
{
	Demuxer* demuxer = (Demuxer*)user_data;
	demuxer->handle_eos(track_num);
}

void Demuxer::demuxer_error_cb (mediademuxer_error_e error, void *user_data)
{
	Demuxer* demuxer = (Demuxer*)user_data;
	demuxer->handle_error(error);
}
