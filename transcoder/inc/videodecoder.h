/*
 * videodecoder.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#ifndef VIDEODECODER_H_
#define VIDEODECODER_H_

#include "codecbase.h"

class VideoDecoder : public CodecBase
{
public:
	VideoDecoder();
	~VideoDecoder();

protected:
	bool create(mediacodec_h mediacodec, const CodecInfo& codecinfo);
	void destroy();
	const char* getname();

};


#endif /* VIDEODECODER_H_ */
