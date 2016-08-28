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
	bool create(mediacodec_h mediacodec, const CodecInfo& codecinfo);

	const char* getname();
	media_format_h create_format(const CodecInfo& codecinfo);


};


#endif /* AUDIOENCODER_H_ */
