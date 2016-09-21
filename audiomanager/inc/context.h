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

/*
 * The Context class places Listener and Source objects within the given 3D space room
 * and mixes audio sources that reflect their relative locations.
 */

#define MAXNUM 5

class Context
{
public:
	Context();
	~Context();
	void Create();
	void Destroy();
	void SetListenerPos(float x, float y, float z);
	void Play();
	void Stop();
	void Push(Source* source);
	void Pop(Source* source);
	void ResetSource();

private:
	void convertVecToArr(ALuint* arr);

private:
	ALCdevice * m_device;
	ALCcontext* m_context;
	std::vector<Source*> m_ImportSourceIdx;
};

#endif /* CONTEXT_H_ */
