/*
 * sharedqueue.h
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#ifndef SHAREDQUEUE_H_
#define SHAREDQUEUE_H_


#include "media_packet.h"
#include <queue>
#include "eina_lock.h"
#include <string>

#define END_OF_STREAM	0

class SharedQueue
{
public:
	SharedQueue();
	~SharedQueue();

	void SetName(const char* queuename);
	void ClearAll();
	bool Push(media_packet_h packet);
	void PushEoS();
	media_packet_h Get();
	media_packet_h JustSee();
	void SetBackToEoS();

	bool IsEmpty();
	bool IsFull();
	unsigned int Size();
	void SetMaxQueueSize(unsigned int size);


private:
	std::queue<media_packet_h> m_queue;
	unsigned int m_max_size;
	Eina_Lock m_mutex;
	std::string m_name;
};

#endif /* SHAREDQUEUE_H_ */
