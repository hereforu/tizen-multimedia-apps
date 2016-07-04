/*
 * sharedqueue.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "base.h"
#include "sharedqueue.h"
#include <stdexcept>



SharedQueue::SharedQueue()
:m_max_size(5000)
{
	create();
}
SharedQueue::~SharedQueue()
{
	destroy();
}
void SharedQueue::ClearAll()
{
	eina_lock_take(&m_mutex);
	while(m_queue.empty()==false)
	{
		media_packet_h packet = m_queue.front();
		if(packet != END_OF_STREAM)
		{
			media_packet_destroy(packet);
		}

		m_queue.pop();
	}
	eina_lock_release(&m_mutex);
}
void SharedQueue::create()
{
	Eina_Bool ret = eina_lock_new(&m_mutex);
	if(ret == EINA_FALSE)
	{
		throw std::runtime_error("fail to create eina_lock_new");
	}

}
void SharedQueue::destroy()
{
	ClearAll();
	eina_lock_free(&m_mutex);
}

bool SharedQueue::Push(media_packet_h packet)
{
	if(IsFull())
		return false;
	eina_lock_take(&m_mutex);
	m_queue.push(packet);
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
	return (m_queue.empty())?true:false;
}
bool SharedQueue::IsFull()
{
	return (m_queue.size()==m_max_size)?true:false;
}
unsigned int SharedQueue::Size()
{
	return m_queue.size();
}


