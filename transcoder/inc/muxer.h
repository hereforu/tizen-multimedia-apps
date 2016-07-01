/*
 * muxer.h
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#ifndef MUXER_H_
#define MUXER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mediademuxer.h"
#include "mediamuxer.h"
#include "media_codec.h"



typedef void (*ReadSampleCB)(void* data, int track_index, media_packet_h* inbuf, bool* eos);

bool CreateMuxer(const char* dstfilepath, mediamuxer_output_format_e format);
void DestroyMuxer();
int AddTrack(media_format_h media_format);
bool StartMuxer(ReadSampleCB readsamplecb, void* data);
//bool StopMuxer();


#ifdef __cplusplus
}
#endif



#endif /* MUXER_H_ */

