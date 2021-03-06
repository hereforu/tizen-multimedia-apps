/*
 * codecbase.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */
#ifndef CDEDCBASE_H_
#define CDEDCBASE_H_

#include "mediadef.h"

typedef struct _ProcessingProperty
{
	_ProcessingProperty()
	:count(0), eos(false)
	{}
	SharedQueue queue;
	unsigned int count;
	bool eos;
}ProcessingProperty;

class CodecBase
{
public:
	CodecBase();
	virtual ~CodecBase();

	void Create(const CodecInfo& codecinfo);
	void Destroy();
	bool GetPacket(media_packet_h& packet);
	bool InsertPacket(media_packet_h packet);
	bool IsEoS();
	media_format_h GetMediaFormat();

protected:
	virtual const char* getname() = 0;
	virtual bool create(mediacodec_h mediacodec, const CodecInfo& codecinfo) = 0;
	virtual media_format_h create_format(const CodecInfo& codecinfo) = 0;


private:
	bool pushpacket_to_outputqueue(const media_packet_h& packet);
	void analyze_packet_for_debug(media_packet_h pkt);
	void handle_input_buffer_used(media_packet_h pkt);
	void handle_output_buffer_available(media_packet_h pkt);
	void handle_error(mediacodec_error_e error);
	void handle_eos();

	static void mc_input_buffer_used_cb(media_packet_h pkt, void *user_data);
	static void mc_output_buffer_available_cb(media_packet_h pkt, void *user_data);
	static void mc_error_cb(mediacodec_error_e error, void *user_data);
	static void mc_eos_cb(void *user_data);

private:
	mediacodec_h m_mediacodec;
	ProcessingProperty m_out;
	unsigned int m_in_count;
	media_format_h m_format;
};

#endif /* CDEDCBASE_H_ */
