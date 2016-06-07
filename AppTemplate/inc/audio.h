/*
 * audio.h
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "media.h"

class Audio : public Media
{
public:
	Audio();
	~Audio();

	void play();
	void pause();
	void stop();

protected:
	void setMediaInfo();

};


#endif /* AUDIO_H_ */
