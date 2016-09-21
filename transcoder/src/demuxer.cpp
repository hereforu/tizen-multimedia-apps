/*
 * demuxer.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "demuxer.h"
#include <stdexcept>

#define throw_error_and_destroy_demuxer(demuxer, msg, error_code)\
	{\
		if(demuxer)\
		{\
			mediademuxer_destroy(demuxer);\
			demuxer = NULL;\
		}\
		throw std::runtime_error(std::string(msg)+to_string(error_code));\
	}\


Demuxer::Demuxer()
:m_videotrackindex(-1), m_audiotrackindex(-1),m_demuxer(NULL)
{

}

Demuxer::~Demuxer()
{
	Destroy();
}

void Demuxer::Create(const char* srcfilename)
{
	int ret = MEDIADEMUXER_ERROR_NONE;
	if((ret = mediademuxer_create(&m_demuxer)) != MEDIADEMUXER_ERROR_NONE)
		throw_error_and_destroy_demuxer(m_demuxer, "fail to create demuxer: ", ret);
	if((ret = mediademuxer_set_eos_cb(m_demuxer, demuxer_eos_cb, (void*)this)) != MEDIADEMUXER_ERROR_NONE)
		throw_error_and_destroy_demuxer(m_demuxer, "fail to mediademuxer_set_eos_cb: ", ret);
	if((ret = mediademuxer_set_error_cb(m_demuxer, demuxer_error_cb, (void*)this)) != MEDIADEMUXER_ERROR_NONE)
		throw_error_and_destroy_demuxer(m_demuxer, "fail to mediademuxer_set_error_cb: ", ret);
	if((ret = mediademuxer_set_data_source(m_demuxer, srcfilename)) != MEDIADEMUXER_ERROR_NONE)
		throw_error_and_destroy_demuxer(m_demuxer, "fail to mediademuxer_set_data_source: ", ret);
	if((ret = mediademuxer_prepare(m_demuxer)) != MEDIADEMUXER_ERROR_NONE)
		throw_error_and_destroy_demuxer(m_demuxer, "fail to mediademuxer_prepare: ", ret);
}

void Demuxer::Destroy()
{
	if(m_demuxer == NULL)
		return;
	dlog_print(DLOG_DEBUG, "Demuxer", "enter into destroy");
	for(int i= 0; i < m_tracks.size(); ++i)
	{
		if(m_tracks[i].info.fmt)
			media_format_unref(m_tracks[i].info.fmt);
	}
	dlog_print(DLOG_DEBUG, "Demuxer", "complete media_format_unref");
	m_tracks.clear();
	mediademuxer_unset_eos_cb(m_demuxer);
	mediademuxer_unset_error_cb(m_demuxer);
	dlog_print(DLOG_DEBUG, "Demuxer", "complete mediademuxer_unset_xxxx");
	//if ready state
	int ret = mediademuxer_unprepare(m_demuxer);
	dlog_print(DLOG_DEBUG, "Demuxer", "complete mediademuxer_unprepare");
	if(ret != MEDIAMUXER_ERROR_NONE)
		dlog_print(DLOG_ERROR, "Demuxer", "fail to mediademuxer_unprepare:%d", ret);

	ret = mediademuxer_destroy(m_demuxer);
	dlog_print(DLOG_DEBUG, "Demuxer", "complete mediademuxer_destroy");
	if(ret != MEDIAMUXER_ERROR_NONE)
		dlog_print(DLOG_ERROR, "Demuxer", "fail to mediademuxer_destroy:%d", ret);
	m_demuxer = NULL;
	dlog_print(DLOG_DEBUG, "Demuxer", "exit from destroy");
}



void Demuxer::Start()
{
	//in this sample code, we support only one video track and one audio track!
	if(m_videotrackindex != -1)
		iferror_throw(mediademuxer_select_track(m_demuxer, m_videotrackindex), "fail to mediademuxer_select_track for video: ");
	if(m_audiotrackindex != -1)
		iferror_throw(mediademuxer_select_track(m_demuxer, m_audiotrackindex), "fail to mediademuxer_select_track for audio: ");
	iferror_throw(mediademuxer_start(m_demuxer), "fail to mediademuxer_start: ");
	//read a sample in advance to check the end of stream manually
	//this code would be removed if demuxer set the EoS flag to the end packet
	prefetch_sample();
}

void Demuxer::ExtractTrackinfo()
{
	int trackcount = 0;
	iferror_throw(mediademuxer_get_track_count(m_demuxer, &trackcount), "fail to mediademuxer_get_track_count: ");
	for(int i= 0; i < trackcount; ++i)
	{
		TrackForDemuxer track;
		iferror_throw(mediademuxer_get_track_info(m_demuxer, i, &track.info.fmt), "fail to mediademuxer_get_track_info: ");
		media_format_type_e formattype;
		int ret = MEDIA_FORMAT_ERROR_NONE;
		if((ret = media_format_get_type(track.info.fmt, &formattype))== MEDIA_FORMAT_ERROR_NONE)
		{
			switch(formattype)
			{
				case MEDIA_FORMAT_VIDEO:
					m_videotrackindex = i;
					m_tracks.push_back(track);
					break;
				case MEDIA_FORMAT_AUDIO:
					m_audiotrackindex = i;
					m_tracks.push_back(track);
					break;
				default:
					dlog_print(DLOG_ERROR, "Demuxer", "unsupportive type (%d)", (int)formattype);
					break;
			}
		}
		else
		{
			dlog_print(DLOG_ERROR, "Demuxer", "fail to media_format_get_type (%d)", ret);
		}
	}
}

bool Demuxer::ReadSeample(int track_index, media_packet_h* packet)
{
	if(m_tracks[track_index].packet != NULL)
	{
		*packet = m_tracks[track_index].packet;
		m_tracks[track_index].packet = NULL;
		read_sample(track_index, &m_tracks[track_index].packet);
		if(m_tracks[track_index].packet == NULL) //there's no next sample, i.e. EoS
		{
			int ret = media_packet_set_flags(*packet, MEDIA_PACKET_END_OF_STREAM);
			m_tracks[track_index].info.eos = true;
			dlog_print(DLOG_DEBUG, "Demuxer", "media_packet_set_flags as EOS %d", ret);
		}
		return true;
	}
	return false;
}

bool Demuxer::IsEoS(int track_index)
{
	return m_tracks[track_index].info.eos;
}

void Demuxer::Stop()
{
	iferror_throw(mediademuxer_stop(m_demuxer), "fail to mediademuxer_stop: ");
	if(m_videotrackindex != -1)
	{
		iferror_throw(mediademuxer_unselect_track(m_demuxer, m_videotrackindex), "fail to mediademuxer_unselect_track for video: ");
	}
	if(m_audiotrackindex != -1)
	{
		iferror_throw(mediademuxer_unselect_track(m_demuxer, m_audiotrackindex), "fail to mediademuxer_unselect_track for audio: ");
	}
}


unsigned int Demuxer::GetNumTracks()
{
	return m_tracks.size();
}

media_format_h Demuxer::GetMediaFormat(int track_index)
{
	return m_tracks[track_index].info.fmt;
}

bool Demuxer::GetVideoDecInfo(CodecInfo& vdec)
{
	if(m_videotrackindex == -1)
		return false;
	media_format_mimetype_e mimetype;
	int width, height, avg_bps, max_bps;
	int ret = media_format_get_video_info(m_tracks[m_videotrackindex].info.fmt, &mimetype, &width, &height, &avg_bps, &max_bps);
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
	int ret = media_format_get_audio_info(m_tracks[m_audiotrackindex].info.fmt, &mimetype, &channel, &samplerate, &bit, &avg_bps);
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

int Demuxer::GetVideoTrackIndex()
{
	return m_videotrackindex;
}

int Demuxer::GetAudioTrackIndex()
{

	return m_audiotrackindex;
}


void Demuxer::prefetch_sample()
{
	if(m_videotrackindex != -1)
		iferror_throw(read_sample(m_videotrackindex, &m_tracks[m_videotrackindex].packet), "fail to read a sample in the prepare stage for video: ");

	if(m_audiotrackindex != -1)
		iferror_throw(read_sample(m_audiotrackindex, &m_tracks[m_audiotrackindex].packet), "fail to read a sample in the prepare stage for audio: ");
}

int Demuxer::read_sample(int track_index, media_packet_h* packet)
{
	if(m_tracks[track_index].info.eos == true)
		return MEDIADEMUXER_ERROR_NONE;
	dlog_print(DLOG_DEBUG, "Demuxer", "enter mediademuxer_read_sample");
	int ret = mediademuxer_read_sample(m_demuxer, track_index, packet);
	dlog_print(DLOG_DEBUG, "Demuxer", "exit from mediademuxer_read_sample(%d)", ret);
	if(ret != MEDIADEMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "Demuxer", "mediademuxer_read_sample(%d)", ret);
		*packet = NULL;
	}
	else
	{
		packet_created_dbg(*packet);
	}
	return ret;
}


void Demuxer::iferror_throw(int ret, const char* msg)
{
	if(ret != MEDIADEMUXER_ERROR_NONE)
		throw std::runtime_error(std::string(msg)+to_string(ret));
}

void Demuxer::handle_eos(int track_num)
{
	dlog_print(DLOG_DEBUG, "Demuxer", "eos for %d track", track_num);
	m_tracks[track_num].info.eos = true;
}

void Demuxer::handle_error(mediademuxer_error_e error)
{
	dlog_print(DLOG_DEBUG, "Demuxer", "mediademuxer_error_e: %d", error);
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
