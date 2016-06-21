/*
 * source.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "source.h"

Source::Source()
{

}

Source::~Source()
{

}

void Source::GenSource(ALuint buffer)
{
	m_buffer = buffer;
	alGenSources(1, &m_source);
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
