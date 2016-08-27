/*
 * videoencoder.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#ifndef VIDEOENCODER_H_
#define VIDEOENCODER_H_


#include "codecbase.h"

class VideoEncoder : public CodecBase
{
public:
	VideoEncoder();
	~VideoEncoder();

protected:
	bool create(mediacodec_h mediacodec, const CodecInfo& codecinfo);
	void destroy();
	const char* getname();
	media_format_h create_format(const CodecInfo& codecinfo);
};


#endif /* VIDEOENCODER_H_ */
