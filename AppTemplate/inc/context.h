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
#include "source.h"
#include <vector>

#define MAXNUM 5

class Context
{
public:
	Context();
	~Context();
	void ResetSource();
	void setSourcePos(ALuint source, int x, int y, int z);
	void setListenerPos(int x, int y, int z);
	void Play();
	void Stop();
	void Push(Source* source);
	void Pop(Source* source);

protected:

private:
	void convertVecToArr(ALuint* arr);

	std::vector<Source*> m_ImportSourceIdx;
};

#endif /* CONTEXT_H_ */
