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
#include <memory>

/*
 * this class supports only the video file which has both one video track and one audio track.
 */
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
		VIDEO_DECODER = 0,
		VIDEO_ENCODER,
		AUDIO_DECODER,
		AUDIO_ENCODER,
		MAX_CODEC
	};

	TranscodingEngine();
	~TranscodingEngine();
	void Destroy();
	void Transcoding(const char* srcfilename, unsigned int duration, const CodecInfo& venc, const CodecInfo& aenc);
	void Cancel();
	double GetProgress();
	const char* GetDstFileName();
	bool IsCanceled();

private:
	void prepare(const char* srcfilename, unsigned int duration, const CodecInfo& venc, const CodecInfo& aenc);
	void unprepare();
	void transcoding();
	void process_track(int track_index, int muxer_track_index, CodecBase* decoder, CodecBase* encoder, int counter[], unsigned int& pts);
	bool feed_decoder_with_packet(CodecBase* decoder, int track_index, int& count, unsigned int& pts);
	bool resize_resolution_if_image(media_packet_h* packet);
	bool feed_encoder_with_packet(CodecBase* decoder, CodecBase* encoder, int& count);
	bool feed_muxer_with_packet(CodecBase* encoder,int muxer_track_index, int& count);
	const char* generatedstfilename(const char* srcfilename);
	void create_demuxer(const char* srcfilename);
	void fill_and_get_codec_info(CodecInfo& vdec, CodecInfo& adec, CodecInfo& venc, CodecInfo& aenc);
	void create_codec(int codectype, const CodecInfo& codecinfo);
	void create_muxer(const char* srcfilename);
	void create_resizer(int target_width, int target_height);
	void print_errorcode_for_debug();
	unsigned int get_pts_in_msec(media_packet_h packet);

private:
	Demuxer* m_demuxer;
	Muxer* m_muxer;
	ImageResizer* m_resizer;
	CodecBase* m_codec[MAX_CODEC];
	std::string m_dstfilename; //auto generation

	int m_progress_count;
	int m_estimated_packets;
	int m_muxer_video_track_index;
	int m_muxer_audio_track_index;
	bool m_bcanceled;
};
//void start_transcoding(const char* srcfilename, CodecInfo& venc, CodecInfo& aenc);
#endif /* TRANSCODINGENGINE_H_ */
