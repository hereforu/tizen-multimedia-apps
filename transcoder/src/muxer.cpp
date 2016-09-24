/*
 * muxer.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "muxer.h"
#include "transcoder.h"
#include <stdexcept>


Muxer::Muxer()
:m_muxer(NULL)
{

}
Muxer::~Muxer()
{
	Destroy();
}

/*
 * At Create, a muxer can be created by calling mediamuxer_create
 * and the output file format and name are specified with mediamuxer_set_data_sink.
 * Note that the file name must not be stored as the local variable
 * because the platform does not allocate additional memory for this purpose
 */
void Muxer::Create(const char* dstfilepath, mediamuxer_output_format_e format)
{
	m_dstfilename = dstfilepath;
	iferror_throw(mediamuxer_create(&m_muxer), "fail to mediamuxer_create");
	int ret = MEDIAMUXER_ERROR_NONE;
	if((ret = mediamuxer_set_data_sink(m_muxer, const_cast<char*>(m_dstfilename.c_str()), format))!= MEDIAMUXER_ERROR_NONE)
	{
		mediamuxer_destroy(m_muxer);
		m_muxer = NULL;
		throw std::runtime_error("fail to mediamuxer_set_data_sinkr");
	}
}

void Muxer::Destroy()
{
	if(m_muxer == NULL)
		return;

	dlog_print(DLOG_DEBUG, "Muxer", "enter into destroy");
	int ret = mediamuxer_destroy(m_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
		dlog_print(DLOG_ERROR, "Muxer", "fail to mediamuxer_destroy:%d", ret);

	m_muxer = NULL;
	dlog_print(DLOG_DEBUG, "Muxer", "exit destroy");
}

/*
 * Once a muxer object is created,
 * audio and video tracks are added using mediamuxer_add_track
 * and the format of each track has to be provided by the video/audio encoder
 */
int Muxer::AddTrack(media_format_h media_format)
{
	int index = -1;
	iferror_throw(mediamuxer_add_track(m_muxer, media_format, &index), "fail to mediamuxer_add_track");
	return index;
}

void Muxer::CloseTrack(int track_index)
{
	iferror_throw(mediamuxer_close_track(m_muxer, track_index), "fail to mediamuxer_close_track");
}

/*
 * Once tracks have been successfully added, muxing can start.
 * The muxer state is set to MEDIA_MUXER_READY_STATE by calling mediamuxer_prepare,
 * followed by calling mediamuxer_start to change the state to RUNNING.
 * Unlike Media Demuxer, only 'start' operation can be performed when the muxer is in the state MEDIA_MUXER_READY_STATE.
 * Once mediamuxer_prepare is successfully performed, mediamuxer_start can be called immediately
 */
void Muxer::Start()
{
	iferror_throw(mediamuxer_prepare(m_muxer), "fail to mediamuxer_prepare");
	int ret = MEDIAMUXER_ERROR_NONE;
	if((ret = mediamuxer_start(m_muxer))!= MEDIAMUXER_ERROR_NONE)
	{
		mediamuxer_unprepare(m_muxer);
		m_muxer = NULL;
		throw std::runtime_error("fail to mediamuxer_start");
	}
}

/*
 * If the muxer is in the state MEDIA_MUXER_RUNNING_STATE, then packets can be written
 * A muxer does not encode video/audio but performs only multiplexing complying to the container format
 * A muxer can get encoded packets using Media Codec
 */
bool Muxer::WriteSample(int track_index, media_packet_h sample)
{
	int ret = MEDIAMUXER_ERROR_NONE;
	dlog_print(DLOG_DEBUG, "Muxer", "enter mediamuxer_write_sample");
	if((ret = mediamuxer_write_sample(m_muxer, track_index, sample))!= MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "Muxer", "fail to mediamuxer_write_sample:%d", ret);
		return false;
	}
	dlog_print(DLOG_DEBUG, "Muxer", "exit mediamuxer_write_sample:%d", ret);
	return true;
}

/*
 * After packets have been all written,
 * the resources are released in the reverse order of creating them as:
 * mediamuxer_stop-> mediamuxer_unprepare -> mediamuxer_destroy
 */
void Muxer::Stop()
{
	int ret = mediamuxer_stop(m_muxer);
	dlog_print(DLOG_DEBUG, "Muxer", "mediamuxer_stop:%d", ret);
	ret = mediamuxer_unprepare(m_muxer);
	dlog_print(DLOG_DEBUG, "Muxer", "mediamuxer_unprepare:%d", ret);
}





