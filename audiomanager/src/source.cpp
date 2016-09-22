/*
 * source.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "common/base.h"
#include "source.h"
#include <dlog.h>

Source::Source()
{
	m_source = 0;
}

Source::~Source()
{
	Destroy();
}

/*
 * GenerateSource creates Source objects through algenSources and links them to Buffer objects.
 * It also determines a few basic properties used in Application.
 */
bool Source::GenerateSource(ALuint buffer)
{
	ALuint source;
	alGenSources(1, &source);
	dlog_print(DLOG_DEBUG, "ALContext", "alGenSources id:%u, buffer:%u", source, buffer);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "ALContext", "alGenSources error:%d", ret);
		return false;
	}

	alGetSourcef(source, AL_MAX_GAIN, &m_max_gain);
	alGetSourcef(source, AL_MIN_GAIN, &m_min_gain);
	alGetSourcef(source, AL_MAX_DISTANCE, &m_max_distance);
	alGetSourcef(source, AL_REFERENCE_DISTANCE, &m_reference_distance);
	dlog_print(DLOG_DEBUG, "ALContext", "MAX_GAIN:%f, MIN_GAIN:%f, MAX_DIST:%f, REF_DIST:%f ", m_max_gain, m_min_gain, m_max_distance, m_reference_distance);
	alSourcef(source, AL_GAIN, m_max_gain);

	//AL_MAX_DISTANCE is set for distance normalization within the 3D space room

	alSourcef(source, AL_MAX_DISTANCE, sqrt(3*MAX_DIST_PER_AXIS*MAX_DIST_PER_AXIS));
	alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSourcei(source,AL_LOOPING,AL_TRUE);
	alSourcei(source, AL_BUFFER, buffer);
	m_source = source;
	return true;
}

/*
 * The function SetSourcePos is used to specify the location of a Source object within the given 3D space.
 * It uses AL_POSITION of alSource3f in OpenAL to achieve this goal.
 */

void Source::SetSourcePos(float x, float y, float z)
{
	alSource3f(m_source, AL_POSITION, x, y, z);
	dlog_print(DLOG_DEBUG, "Source", "source:%u, (%f, %f, %f)", m_source, x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "Source", "alSource3f error:%d", ret);
	}
}

/*
 * This function can be used to play this source alone
 */
void Source::Play()
{
	alSourcePlay(m_source);
}

void Source::Destroy()
{
	if(m_source != 0)
	{
		alDeleteSources(1, &m_source);
		m_source = 0;
	}
}

ALuint Source::GetSourceId()
{
	return m_source;
}
