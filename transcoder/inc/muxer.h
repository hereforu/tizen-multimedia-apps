/*
 * muxer.h
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#ifndef MUXER_H_
#define MUXER_H_


#include "mediademuxer.h"
#include "mediamuxer.h"
#include "media_codec.h"
#include <string>

//should be refactoring!!!

class Muxer
{
public:
	Muxer();
	~Muxer();

	void Create(const char* dstfilepath, mediamuxer_output_format_e format);
	void Destroy();

	int AddTrack(media_format_h media_format);
	void CloseTrack(int track_index);

	void Start();
	bool WriteSample(int track_index, media_packet_h sample);
	void Stop();

private:
	mediamuxer_h m_muxer;
	std::string m_dstfilename;
};

#endif /* MUXER_H_ */

