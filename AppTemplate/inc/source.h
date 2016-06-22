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
	void Destroy();
	ALuint GetSourceId();
protected:

private:
	ALuint m_source;
	ALuint m_buffer;
};

#endif /* SOURCE_H_ */
