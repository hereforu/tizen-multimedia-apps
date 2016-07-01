/*
 * demuxer.h
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#ifndef DEMUXER_H_
#define DEMUXER_H_



#include "mediadef.h"
#include <vector>

class Demuxer
{
public:
	Demuxer();
	~Demuxer();

	void Create(const char* srcfilename);
	void Start();
	void Destroy();

	unsigned int GetNumTracks();
	media_format_h GetMediaFormat(int track_index);
	void SetQueue(int track_index, SharedQueue* queue);


private:
	void demuxing(int track_index);
	void extract_tracks();
	void iferror_throw(int ret, const char* msg);
	void handle_eos(int track_num);
	void handle_error(mediademuxer_error_e error);
	static void demuxer_eos_cb(int track_num, void *user_data);
	static void demuxer_error_cb (mediademuxer_error_e error, void *user_data);
private:
	mediademuxer_h m_demuxer;
	std::vector<TrackInfo> m_tracks;
	bool m_eosflag;

};


#endif /* DEMUXER_H_ */
