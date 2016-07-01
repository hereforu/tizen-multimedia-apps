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
//media_packet_h
class SharedQueue
{
public:
	SharedQueue();
	~SharedQueue();

	void Create();
	void Destroy();

	bool Push(media_packet_h packet);
	media_packet_h Get();

	bool IsEmpty();
	bool IsFull();
	int Size();

private:
	std::queue<media_packet_h> m_queue;
	const int m_max_size;
	Eina_Lock m_mutex;
};

#endif /* SHAREDQUEUE_H_ */
