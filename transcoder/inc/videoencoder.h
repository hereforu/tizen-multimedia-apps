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
	media_format_h GetMediaFormat();
protected:
	void create(mediacodec_h mediacodec, const CodecInfo& codecinfo);
	void destroy();
	const char* getname();

private:
	media_format_h m_format;



};


#endif /* VIDEOENCODER_H_ */
