/*
 * context.cpp

 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */

#include "context.h"
#include <dlog.h>
#include <stdexcept>

Context::Context()
:m_device(NULL), m_context(NULL)
{
}

Context::~Context()
{
}

void Context::Create()
{
	const ALCchar* defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	if(defaultDeviceName == NULL)
	{
		throw std::runtime_error("fail to alcGetString");
	}
	m_device = alcOpenDevice(defaultDeviceName);
	if(m_device == NULL)
	{
		throw std::runtime_error("fail to alcOpenDevice");
	}
	ALCint attribs[] = {ALC_HRTF_SOFT, ALC_TRUE, 0 };
	m_context = alcCreateContext(m_device, attribs);
	if(m_context == NULL)
	{
		Destroy();
		throw std::runtime_error("fail to alcCreateContext");
	}
	if(alcMakeContextCurrent(m_context) == ALC_FALSE)
	{
		Destroy();
		throw std::runtime_error("fail to alcMakeContextCurrent");
	}
}

void Context::Destroy()
{
	//you don't need to delete source objects here since they are managed by model object
	alcMakeContextCurrent(NULL);
	if(m_context)
	{
		alcDestroyContext(m_context);
		m_context = NULL;
	}
	if(m_device)
	{
		alcCloseDevice(m_device);
		m_device = NULL;
	}
}
void Context::ResetSource()
{
	m_ImportSourceIdx.clear();
}

void Context::convertVecToArr(ALuint* arr)
{
	for(int i = 0 ; i < m_ImportSourceIdx.size() ; i++)
	{
		arr[i] = m_ImportSourceIdx[i]->GetSourceId();
	}
}



void Context::setSourcePos(ALuint source, float x, float y, float z)
{
	alSource3f(source, AL_POSITION, x, y, z);
	dlog_print(DLOG_DEBUG, "ALContext", "source:%u, (%f, %f, %f)", source, x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "ALContext", "alSource3f error:%d", ret);
	}
}

void Context::setListenerPos(float x, float y, float z)
{
	alListener3f(AL_POSITION, x, y, z);
	dlog_print(DLOG_DEBUG, "ALContext", "listener:(%f, %f, %f)", x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "ALContext", "alListener3f error:%d", ret);
	}
}

void Context::Play()
{
	ALuint source[MAXNUM]={0, };
	convertVecToArr(source);
	alSourcePlayv(m_ImportSourceIdx.size(), source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "ALContext", "alSourcePlayv error:%d", ret);
	}
}

void Context::Stop()
{
	ALuint source[MAXNUM]={0, };
	convertVecToArr(source);
	alSourceStopv(m_ImportSourceIdx.size(), source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "ALContext", "alSourceStopv error:%d", ret);
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
