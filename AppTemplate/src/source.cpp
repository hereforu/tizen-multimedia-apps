/*
 * source.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "source.h"
#include <dlog.h>

Source::Source()
{
	m_source = 0;
}

Source::~Source()
{

}

void Source::GenerateSource(ALuint buffer)
{
	ALuint source;
	alGenSources(1, &source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "GenerateSource", "error");
	}
	m_source = source;
	alSourcef(m_source, AL_GAIN, 100);
	alSource3f(m_source, AL_POSITION, 0, 0, 0);
	alSourcei(m_source, AL_BUFFER, buffer);
}

void Source::Play()
{
	alSourcePlay(m_source);
}

void Source::Destroy()
{
	alDeleteSources(1, &m_source);
}

ALuint Source::GetSourceId()
{
	return m_source;
}
