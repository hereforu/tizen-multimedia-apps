/*
 * videoplayer.h
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */

#ifndef VIDEOPLAYER_H_
#define VIDEOPLAYER_H_

#include "player.h"
#include <string>

typedef struct _VideoInfo
{
	std::string video_codec;
	std::string audio_codec;
	int fps;
	int video_bitrate;
}VideoInfo;

class VideoPlayer
{
public:
	VideoPlayer();
	~VideoPlayer();

	void Create();
	void Destroy();
	void Play();
	void Stop();

	void SetMediaAndPrepare(const char* filepath, Evas_Object* display);
	VideoInfo GetVideoCodecInfo();



protected:

private:
	VideoInfo m_videoinfo;
	player_h m_player;

};


#endif /* VIDEOPLAYER_H_ */
