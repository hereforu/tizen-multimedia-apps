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
	void create(mediacodec_h mediacodec, const CodecInfo& codecinfo);
	void destroy();


private:




};


#endif /* AUDIODECODER_H_ */
