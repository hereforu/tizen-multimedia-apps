/*
 * codecbase.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */
#ifndef CDEDCBASE_H_
#define CDEDCBASE_H_

#include "mediadef.h"

class CodecBase
{
public:
	CodecBase();
	virtual ~CodecBase();

	void Create(const CodecInfo& codecinfo, SharedQueue* inputqueue, SharedQueue* outputqueue);
	void Destroy();

	void Start();
	void Stop();

	bool IsEoS();
	media_format_h GetMediaFormat();

protected:

	mediacodec_h getmediacodec();
	void pushpacket_to_outputqueue(const media_packet_h& packet);
	void pusheos_to_outputqueue();
	bool getpacket_from_inputqueue(media_packet_h& packet);
	bool is_next_packet_eos_from_inputqueue();
	virtual void create(mediacodec_h mediacodec, const CodecInfo& codecinfo) = 0;
	virtual void destroy() = 0;

private:
	void iferror_throw(int ret, const char* msg);

	void handle_input_buffer_used(media_packet_h pkt);
	void handle_output_buffer_available(media_packet_h pkt);
	void handle_error(mediacodec_error_e error);
	void handle_eos();
	static void mc_input_buffer_used_cb(media_packet_h pkt, void *user_data);
	static void mc_output_buffer_available_cb(media_packet_h pkt, void *user_data);
	static void mc_error_cb(mediacodec_error_e error, void *user_data);
	static void mc_eos_cb(void *user_data);

protected:
	media_format_h m_format;
private:
	mediacodec_h m_mediacodec;
	SharedQueue* m_inputqueue;
	SharedQueue* m_outputqueue;
	bool m_eos;
	const int m_watingtime;
	int m_processingcount;
};

#endif /* CDEDCBASE_H_ */
