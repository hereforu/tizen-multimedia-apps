/*
 * codecbase.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#include "base.h"
#include "codecbase.h"
#include <stdexcept>


CodecBase::CodecBase()
:m_mediacodec(NULL), m_inputqueue(NULL), m_outputqueue(NULL), m_eos(false), m_watingtime(1), m_processingcount(0)
{

}
CodecBase::~CodecBase()
{

}

void CodecBase::Create(const CodecInfo& codecinfo, SharedQueue* inputqueue, SharedQueue* outputqueue)
{
	iferror_throw(mediacodec_create(&m_mediacodec), "fail to create mediacodec_create");
	try
	{
		create(m_mediacodec, codecinfo); //codec info setting
	}
	catch(const std::runtime_error& e)
	{
		throw e;
	}
	iferror_throw(mediacodec_set_eos_cb(m_mediacodec, mc_eos_cb, (void*)this), "fail to mediacodec_set_eos_cb");
	iferror_throw(mediacodec_set_error_cb(m_mediacodec, mc_error_cb, (void*)this), "fail to mediacodec_set_error_cb");
	iferror_throw(mediacodec_set_input_buffer_used_cb(m_mediacodec, mc_input_buffer_used_cb, (void*)this), "fail to mediacodec_set_input_buffer_used_cb");
	iferror_throw(mediacodec_set_output_buffer_available_cb(m_mediacodec, mc_output_buffer_available_cb, (void*)this), "fail to mediacodec_set_output_buffer_available_cb");

	iferror_throw(mediacodec_prepare(m_mediacodec), "fail to mediacodec_create");

	m_inputqueue = inputqueue;
	m_outputqueue = outputqueue;
}

void CodecBase::Destroy()
{
	destroy();
	iferror_throw(mediacodec_unprepare(m_mediacodec), "fail to mediacodec_unprepare");
	iferror_throw(mediacodec_destroy(m_mediacodec), "fail to mediacodec_destroy");
	m_mediacodec = NULL;
	m_inputqueue = NULL;
	m_outputqueue = NULL;
};

void CodecBase::Start()
{
	int i = 0;
	bool inputeos = false;
	while(inputeos==false)
	{
		int ret = MEDIACODEC_ERROR_NONE;
		media_packet_h in_buf = NULL;
		if(getpacket_from_inputqueue(in_buf))
		{
			if(is_next_packet_eos_from_inputqueue())
			{
				ret = media_packet_set_flags(in_buf, MEDIA_PACKET_END_OF_STREAM);
				dlog_print(DLOG_DEBUG, "CodecBase", "media_packet_set_flags as EOS%d", ret);
				inputeos = true;
			}
			ret = mediacodec_process_input(m_mediacodec, in_buf, 0);
			if(ret != MEDIACODEC_ERROR_NONE)
			{
				dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_process_input %d", ret);
			}
			else
			{
				dlog_print(DLOG_DEBUG, "CodecBase", "input %d th packet [%p]", i++, this);
			}
		}
		else
		{
			dlog_print(DLOG_DEBUG, "CodecBase", "input queue is empty [%p]", this);
			sleep(1);
		}
	}
}
void CodecBase::Stop()
{

}

bool CodecBase::IsEoS()
{
	return m_eos;
}
media_format_h CodecBase::GetMediaFormat()
{
	return m_format;
}
mediacodec_h CodecBase::getmediacodec()
{
	return m_mediacodec;
}

void CodecBase::pushpacket_to_outputqueue(const media_packet_h& packet)
{
	while(m_outputqueue->IsFull())
		sleep(m_watingtime);
	m_outputqueue->Push(packet);
}
void CodecBase::pusheos_to_outputqueue()
{
	while(m_outputqueue->IsFull())
		sleep(m_watingtime);
	media_packet_h packet = END_OF_STREAM;
	m_outputqueue->Push(packet);
}
bool CodecBase::getpacket_from_inputqueue(media_packet_h& packet)
{
	if(m_inputqueue->IsEmpty())
		return false;
	packet = m_inputqueue->Get();
	return true;
}
bool CodecBase::is_next_packet_eos_from_inputqueue()
{
	if(m_inputqueue->IsEmpty())
		return false;
	media_packet_h packet = m_inputqueue->JustSee();
	if(packet == END_OF_STREAM)
	{
		dlog_print(DLOG_DEBUG, "CodecBase", "EOS from inputqueue");
		packet = m_inputqueue->Get();
		return true;
	}
	return false;
}

void CodecBase::handle_input_buffer_used(media_packet_h pkt)
{
	 media_packet_destroy(pkt);
}
void CodecBase::handle_output_buffer_available(media_packet_h pkt)
{
	media_packet_h output_buf = NULL;
	int ret = mediacodec_get_output(m_mediacodec, &output_buf, 0);
	if(ret != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_get_output %d", ret);
	}
	dlog_print(DLOG_DEBUG, "CodecBase", "mediacodec[%p]_get_output %d", this, m_processingcount++);
	pushpacket_to_outputqueue(output_buf);
}

void CodecBase::handle_error(mediacodec_error_e error)
{
	dlog_print(DLOG_ERROR, "CodecBase", "media codec error %d", (int)error);
}
void CodecBase::handle_eos()
{
	m_eos = true;
	pusheos_to_outputqueue();
}

void CodecBase::mc_input_buffer_used_cb(media_packet_h pkt, void *user_data)
{
	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_input_buffer_used(pkt);
}
void CodecBase::mc_output_buffer_available_cb(media_packet_h pkt, void *user_data)
{
	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_output_buffer_available(pkt);
}
void CodecBase::mc_error_cb(mediacodec_error_e error, void *user_data)
{
	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_error(error);
}
void CodecBase::mc_eos_cb(void *user_data)
{
	dlog_print(DLOG_DEBUG, "CodecBase", "EOS callback");
	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_eos();
}
void CodecBase::iferror_throw(int ret, const char* msg)
{
	if(ret != MEDIACODEC_ERROR_NONE)
		throw std::runtime_error(std::string(msg)+AppTool::ToString(ret));
}
