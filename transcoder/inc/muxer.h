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

//should be refactoring!!!


bool CreateMuxer(const char* dstfilepath, mediamuxer_output_format_e format);
void DestroyMuxer();
int AddTrack(media_format_h media_format);
bool StartMuxer();
bool WriteSample(int track_index, media_packet_h sample);
bool CloseTrack(int track_index);
bool StopMuxer();


#ifdef __cplusplus
}
#endif



#endif /* MUXER_H_ */

