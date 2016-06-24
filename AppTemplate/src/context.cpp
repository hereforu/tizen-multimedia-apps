/*
 * context.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "context.h"
#include <dlog.h>

Context::Context()
{

}

Context::~Context()
{

}

void Context::convertVecToArr(ALuint* arr)
{
	for(int i = 0 ; i < m_ImportSourceIdx.size() ; i++)
	{
		arr[i] = m_ImportSourceIdx[i]->GetSourceId();
	}
}

void Context::ResetSource()
{
	m_ImportSourceIdx.clear();
}

void Context::setSourcePos(ALuint source, int x, int y, int z)
{
	alSource3i(source, AL_POSITION, x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "Context setSourcePos", "error");
	}
}

void Context::setListenerPos(int x, int y, int z)
{
	alListener3i(AL_POSITION, x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "Context setListenerPos", "error");
	}
}

void Context::Play()
{
	ALuint source[MAXNUM];
	convertVecToArr(source);
	alSourcePlayv(m_ImportSourceIdx.size(), source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "Context Play", "error");
	}
}

void Context::Stop()
{
	ALuint source[MAXNUM];
	convertVecToArr(source);
	alSourceStopv(m_ImportSourceIdx.size(), source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "Context Stop", "error");
	}
}

void Context::Push(Source* source)
{
	m_ImportSourceIdx.push_back(source);
}

void Context::Pop(Source* source)
{
	for(int i = 0 ; i < m_ImportSourceIdx.size() ; i++)
	{
		if(m_ImportSourceIdx[i] == source)
		{
			m_ImportSourceIdx.erase(m_ImportSourceIdx.begin() + i);
			break;
		}
	}
}
