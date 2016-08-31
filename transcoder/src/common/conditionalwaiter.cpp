/*
 * conditionalwaiter.cpp
 *
 *  Created on: Aug 31, 2016
 *      Author: Jason
 */
#include "common/conditionalwaiter.h"
#include <stdexcept>


ConditionalWaiter::ConditionalWaiter()
	:m_mutex_created(false), m_condition_created(false)
{
	if(eina_lock_new(&m_mutex)==EINA_FALSE)
	{
		throw std::runtime_error("fail to create eina_lock_new");
	}
	if(eina_condition_new(&m_condition, &m_mutex)==EINA_FALSE)
	{
		eina_lock_free(&m_mutex);
		throw std::runtime_error("fail to create eina_lock_new");
	}
	m_mutex_created = true;
	m_condition_created = true;
}
ConditionalWaiter::~ConditionalWaiter()
{
	if(m_condition_created)
	{
		eina_condition_free(&m_condition);
		m_condition_created = false;
	}
	if(m_mutex_created)
	{
		eina_lock_free(&m_mutex);
		m_mutex_created = false;
	}
}

void ConditionalWaiter::Wait()
{
	eina_condition_wait(&m_condition);
}
void ConditionalWaiter::Signal()
{
	eina_condition_signal(&m_condition);
}
