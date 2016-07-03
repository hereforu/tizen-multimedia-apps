/*
 * transcodingengine.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#ifndef TRANSCODINGENGINE_H_
#define TRANSCODINGENGINE_H_

#include "mediadef.h"
#include "demuxer.h"
#include "videodecoder.h"
#include "videoencoder.h"
#include "audiodecoder.h"
#include "audioencoder.h"


class TranscodingEngine
{
public:
	enum
	{
		AFTER_DEMUXING_QUEUE = 0,
		AFTER_DECODING_QUEUE,
		AFTER_ENCODING_QUEUE,
		MAX_QUEUE
	};
	enum
	{
		VIDEO_TRACK = 0,
		AUDIO_TRACK,
		MAX_TRACKS
	};
	TranscodingEngine();
	~TranscodingEngine();

	void Create(const char* srcfilename, CodecInfo& venc, CodecInfo& aenc);
	void Destroy();
	bool IsCreated();

	void Start();
	void Stop();
	int GetProgress();

private:
	const char* generatedstfilename(const char* srcfilename);
	void createdemuxer(const char* srcfilename);
	void createcodec(CodecInfo& venc, CodecInfo& aenc);

private:
	Demuxer m_demuxer;
	VideoDecoder m_vdecoder;
	VideoEncoder m_vencoder;
	AudioDecoder m_adecoder;
	AudioEncoder m_aencoder;

	SharedQueue m_queue[MAX_QUEUE][MAX_TRACKS];
	std::string m_dstfilename; //auto generation
	CodecInfo m_vencinfo;
	CodecInfo m_aencinfo;
	bool m_bcreated;
};
//void start_transcoding(const char* srcfilename, CodecInfo& venc, CodecInfo& aenc);
#endif /* TRANSCODINGENGINE_H_ */
