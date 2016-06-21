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
	void Push(ALuint source);
	void Pop(ALuint source);

protected:

private:
	void ConvertVecToArr(ALuint* arr);

	std::vector<ALuint> m_ImportSourceIdx;
};

#endif /* CONTEXT_H_ */
