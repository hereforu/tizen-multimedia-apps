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
	m_buffer = 0;
}

Source::~Source()
{

}

void Source::GenerateSource(ALuint buffer)
{
	m_buffer = buffer;
	ALuint source;
	alGenSources(1, &source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "GenerateSource", "error");
	}
	m_source = source;
	//alGenSources(1, &m_source);
	alSourcef(m_source, AL_GAIN, 1);
	alSource3f(m_source, AL_POSITION, 0, 0, 0);
	alSourcei(m_source, AL_BUFFER, m_buffer);
}

void Source::Destroy()
{
	alDeleteBuffers(1, &m_buffer);
	alDeleteSources(1, &m_source);
}

ALuint Source::GetSourceId()
{
	return m_source;
}
