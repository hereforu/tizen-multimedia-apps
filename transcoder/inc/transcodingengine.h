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
#include "muxer.h"
#include "videodecoder.h"
#include "videoencoder.h"
#include "audiodecoder.h"
#include "audioencoder.h"
#include "imageresizer.h"


class TranscodingEngine
{
public:
	enum
	{
		DEMUX_COUNTER = 0,
		DECODE_COUNTER,
		ENCODE_COUNTER,
		MAX_COUNTER
	};
	enum
	{
		VIDEO_TRACK = 0,
		AUDIO_TRACK,
		MAX_TRACKS
	};
	TranscodingEngine();
	~TranscodingEngine();
	void Create(const char* srcfilename, unsigned int duration, CodecInfo& venc, CodecInfo& aenc);
	void Destroy();
	bool IsCreated();
	void Start();
	void Stop();
	double GetProgress();

private:
	void transcoding();
	void process_track(int track_index, int muxer_track_index, CodecBase* decoder, CodecBase* encoder, int counter[], unsigned int& pts);
	bool feed_decoder_with_packet(CodecBase* decoder, int track_index, int& count, unsigned int& pts);
	bool feed_encoder_with_packet(CodecBase* decoder, CodecBase* encoder, int& count);
	bool feed_muxer_with_packet(CodecBase* encoder,int muxer_track_index, int& count);
	const char* generatedstfilename(const char* srcfilename);
	void createdemuxer(const char* srcfilename);
	void createvideocodec(CodecInfo& venc);
	void createaudiocodec(CodecInfo& aenc);
	void createmuxer(const char* srcfilename);
	bool isaudioavailable();
	void print_errorcode_for_debug();

private:
	int m_progress_count;
	int m_estimated_packets;
	Demuxer m_demuxer;
	Muxer m_muxer;
	VideoDecoder m_vdecoder;
	VideoEncoder m_vencoder;
	AudioDecoder m_adecoder;
	AudioEncoder m_aencoder;
	ImageResizer m_resizer;

	std::string m_dstfilename; //auto generation
	CodecInfo m_vencinfo;
	CodecInfo m_aencinfo;
	int m_muxer_video_track_index;
	int m_muxer_audio_track_index;
	bool m_bcreated;
};
//void start_transcoding(const char* srcfilename, CodecInfo& venc, CodecInfo& aenc);
#endif /* TRANSCODINGENGINE_H_ */
