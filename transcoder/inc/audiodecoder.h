/*
 * audiodecoder.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#ifndef AUDIODECODER_H_
#define AUDIODECODER_H_



#include "codecbase.h"

class AudioDecoder : public CodecBase
{
public:
	AudioDecoder();
	~AudioDecoder();

protected:
	bool create(mediacodec_h mediacodec, const CodecInfo& codecinfo);
	void destroy();
	const char* getname();



};


#endif /* AUDIODECODER_H_ */
