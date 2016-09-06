/*
 * source.h
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */

#ifndef SOURCE_H_
#define SOURCE_H_

#include <AL/al.h>
#include <AL/alc.h>

class Source
{
public:
	Source();
	~Source();

	void GenerateSource(ALuint buffer);
	void SetSourcePos(float x, float y, float z);
	void Destroy();
	void Play();
	ALuint GetSourceId();

protected:

private:
	ALuint m_source;
	ALfloat m_min_gain;
	ALfloat m_max_gain;
	ALfloat m_max_distance;
	ALfloat m_reference_distance;
};

#endif /* SOURCE_H_ */
