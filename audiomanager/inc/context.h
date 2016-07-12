/*
 * context.h
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>


#include "source.h"
#include <vector>

#define MAXNUM 5

class Context
{
public:
	Context();
	~Context();

	void ResetSource();
	void setSourcePos(ALuint source, float x, float y, float z);
	void setListenerPos(float x, float y, float z);
	void Play();
	void Stop();
	void Push(Source* source);
	void Pop(Source* source);

private:
	void convertVecToArr(ALuint* arr);

private:
	ALCdevice * m_device;
	ALCcontext* m_contextID;
	std::vector<Source*> m_ImportSourceIdx;
};

#endif /* CONTEXT_H_ */
