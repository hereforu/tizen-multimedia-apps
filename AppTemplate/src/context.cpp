/*
 * context.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "context.h"

Context::Context()
{

}

Context::~Context()
{

}

void Context::ConvertVecToArr(ALuint* arr)
{
	for(int i = 0 ; i < m_ImportSourceIdx.size() ; i++)
	{
		arr[i] = m_ImportSourceIdx[i];
	}
}

void Context::ResetSource()
{
	m_ImportSourceIdx.clear();
}

void Context::setSourcePos(ALuint source, int x, int y, int z)
{
	//return when float overflow
	alSource3i(source, AL_POSITION, x, y, z);
}

void Context::setListenerPos(int x, int y, int z)
{
	//return when float overflow
	alListener3i(AL_POSITION, x, y, z);
}

void Context::Play()
{
	ALuint source[MAXNUM];
	ConvertVecToArr(source);
	//check each source state
	alSourcePlayv(m_ImportSourceIdx.size(), source);
}

void Context::Stop()
{
	ALuint source[MAXNUM];
	ConvertVecToArr(source);
	//check each source state
	alSourceStopv(m_ImportSourceIdx.size(), source);
}

void Context::Push(ALuint source)
{
	m_ImportSourceIdx.push_back(source);
}

void Context::Pop(ALuint source)
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
