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
:m_demuxer(NULL)
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
void Demuxer::SetQueue(int track_index, SharedQueue* queue)
{
	m_tracks[track_index].queue = queue;
}

void Demuxer::demuxing(int track_index)
{
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
			usleep(500);
		m_tracks[track_index].queue->Push(packet);
		++numsample;
	}
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
		m_tracks.push_back(track);
	}
}
void Demuxer::iferror_throw(int ret, const char* msg)
{
	if(ret != MEDIADEMUXER_ERROR_NONE)
		throw std::runtime_error(std::string(msg)+AppTool::ToString(ret));
}

void Demuxer::handle_eos(int track_num)
{
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
