/*
 * sharedqueue.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "sharedqueue.h"
#include <stdexcept>



SharedQueue::SharedQueue()
:m_max_size(100), m_name("queue")
{
	Eina_Bool ret = eina_lock_new(&m_mutex);
	if(ret == EINA_FALSE)
	{
		throw std::runtime_error("fail to create eina_lock_new");
	}
}

SharedQueue::~SharedQueue()
{
	ClearAll();
	eina_lock_free(&m_mutex);
}

void SharedQueue::SetName(const char* queuename)
{
	m_name = queuename;
}

void SharedQueue::SetMaxQueueSize(unsigned int size)
{
	m_max_size = size;
}
void SharedQueue::ClearAll()
{
	eina_lock_take(&m_mutex);
	while(m_queue.empty()==false)
	{
		media_packet_h packet = m_queue.front();
		if(packet != END_OF_STREAM)
		{
			media_packet_destroy_dbg(packet);
		}

		m_queue.pop();
	}
	eina_lock_release(&m_mutex);
}


bool SharedQueue::Push(media_packet_h packet)
{
	if(IsFull())
		return false;
	eina_lock_take(&m_mutex);
	m_queue.push(packet);
	dlog_print(DLOG_DEBUG, "SharedQueue", "current queue:%d [%s]", (int)m_queue.size(), m_name.c_str());
	eina_lock_release(&m_mutex);
	return true;
}

void SharedQueue::PushEoS()
{
	media_packet_h packet = END_OF_STREAM;
	eina_lock_take(&m_mutex);
	m_queue.push(packet);
	eina_lock_release(&m_mutex);
}
void SharedQueue::SetBackToEoS()
{
	eina_lock_take(&m_mutex);
	if(m_queue.empty() == false)
	{
		int ret = media_packet_set_flags(m_queue.back(), MEDIA_PACKET_END_OF_STREAM);
		dlog_print(DLOG_DEBUG, "SharedQueue", "media_packet_set_flags as EOS%d [%s]", ret, m_name.c_str());
	}
	eina_lock_release(&m_mutex);
}
media_packet_h SharedQueue::Get()
{
	eina_lock_take(&m_mutex);
	media_packet_h packet = m_queue.front();
	m_queue.pop();
	eina_lock_release(&m_mutex);

	return packet;
}
media_packet_h SharedQueue::JustSee()
{
	eina_lock_take(&m_mutex);
	media_packet_h packet = m_queue.front();
	eina_lock_release(&m_mutex);

	return packet;
}

bool SharedQueue::IsEmpty()
{
	if(m_queue.empty())
	{
		dlog_print(DLOG_DEBUG,"CodecBase" ,"%s is empty",m_name.c_str());
		return true;
	}
	return false;
}
bool SharedQueue::IsFull()
{
	if(m_queue.size()==m_max_size)
	{
		dlog_print(DLOG_DEBUG, "CodecBase", "%s is full",m_name.c_str());
		return true;
	}
	return false;
}
unsigned int SharedQueue::Size()
{
	return m_queue.size();
}


