/*
 * audioencoder.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#ifndef AUDIOENCODER_H_
#define AUDIOENCODER_H_


#include "codecbase.h"

class AudioEncoder : public CodecBase
{
public:
	AudioEncoder();
	~AudioEncoder();

protected:
	void create(mediacodec_h mediacodec, const CodecInfo& codecinfo);
	void destroy();


private:




};


#endif /* AUDIOENCODER_H_ */
