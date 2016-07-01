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

}
SharedQueue::~SharedQueue()
{

}

void SharedQueue::Create()
{
	Eina_Bool ret = eina_lock_new(&m_mutex);
	if(ret == EINA_FALSE)
	{
		throw std::runtime_error("fail to create eina_lock_new");
	}

}
void SharedQueue::Destroy()
{
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

media_packet_h SharedQueue::Get()
{
	eina_lock_take(&m_mutex);
	media_packet_h packet = m_queue.front();
	m_queue.pop();
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
int SharedQueue::Size()
{
	return m_queue.size();
}


