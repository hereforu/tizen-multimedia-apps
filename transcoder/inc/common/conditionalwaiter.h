/*
 * conditionalwaiter.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Jason
 */

#ifndef CONDITIONALWAITER_H_
#define CONDITIONALWAITER_H_


#include "eina_lock.h"
class ConditionalWaiter
{
public:
	ConditionalWaiter();
	~ConditionalWaiter();
	void Wait();
	void Signal();

private:
	Eina_Lock m_mutex;
	Eina_Condition m_condition;
	bool m_mutex_created;
	bool m_condition_created;

};



#endif /* CONDITIONALWAITER_H_ */
