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

/*
 * The function Create generates a Device object and a Context object for initializing OpenAL.
 */
void Context::Create()
{
	const ALCchar* defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	if(defaultDeviceName == NULL)
	{
		throw std::runtime_error("fail to alcGetString");
	}
	//it opens the default device
	m_device = alcOpenDevice(defaultDeviceName);
	if(m_device == NULL)
	{
		throw std::runtime_error("fail to alcOpenDevice");
	}
	/*
	 * It creates a Context object used in the Default Device with the specified attributes
	 * Head-Related Transfer Function (HRTF) is a method of mixing 3D audio for true 3D panning
	 * typically using filters designed to simulate how sound is affected by a listener's head as the sound waves travel between the ears
	 */
	ALCint attribs[] = {ALC_HRTF_SOFT, ALC_TRUE, 0 };
	m_context = alcCreateContext(m_device, attribs);
	if(m_context == NULL)
	{
		Destroy();
		throw std::runtime_error("fail to alcCreateContext");
	}
	//It activates the created Context object.
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

/*
 * The function convertVecToArr takes the ID of each managed Source object and produces an array of Source object IDs.
 * The funciton Stop can be implemented in a similar fashion.
 */
void Context::convertVecToArr(ALuint* arr)
{
	for(int i = 0 ; i < m_ImportSourceIdx.size() ; i++)
	{
		arr[i] = m_ImportSourceIdx[i]->GetSourceId();
	}
}

/*
 * The function SetListenerPos is called when specifying the location of a given Listener object in the 3D space room.
 * alListener3f is exploited to accomplish this
 */

void Context::SetListenerPos(float x, float y, float z)
{
	alListener3f(AL_POSITION, x, y, z);
	dlog_print(DLOG_DEBUG, "ALContext", "listener:(%f, %f, %f)", x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "ALContext", "alListener3f error:%d", ret);
	}
}

/*
 * The function Play reflects the relative placement of each Source object within the given Context during playback.
 * Source ID array is sent to alSourcePlayv function
 */
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

/*
 * Source objects are managed through list, so Push and Pop functions are available for use.
 */
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
