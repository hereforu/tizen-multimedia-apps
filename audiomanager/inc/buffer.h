/*
 * buffer.h
 *
 *  Created on: June 21, 2016
 *      Author: Hotak
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <AL/al.h>
#include <AL/alc.h>
#include <dlog.h>
#include <stdio.h>
#include <audio_io.h>
#include <string>


#define AL_FORMAT_MONO8                           0x1100
#define AL_FORMAT_MONO16                          0x1101
#define AL_FORMAT_STEREO8                         0x1102
#define AL_FORMAT_STEREO16                        0x1103

struct RIFF_Header {
	char chunkID[4];
	int chunkSize;
	char format[4];
};

struct WAVE_Format {
	char subChunkID[4];
	int subChunkSize;
	short audioFormat;
	short numChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerSample;
};

struct WAVE_Data {
	char subChunkID[4];
	int subChunk2Size;
};

typedef struct _SUB_FORMAT_INFO {
	int sampleRate;
	int format;
}SUB_FORMAT_INFO;

class Buffer
{
public:
	Buffer();
	~Buffer();
	void Destroy();
	int GetDataSize();
	int GetFormat(short numChannels, short bitsPerSample);
	bool GenerateBuffer(const char* wavefilepath);
	ALuint GetBufferID();

protected:

private:
	void release_resources();
	bool readRiffHeader(FILE* waveFile);
	int readwaveformat_and_get_chunksize(SUB_FORMAT_INFO* formatInfo, FILE* waveFile);
	int getFormat(short numChannels, short bitsPerSample);
	int readwavedatainfo_and_get_wavefilesize(int subChunkSize, FILE* waveFile);
	bool parseWave(SUB_FORMAT_INFO* formatInfo, const char* wavefilepath);

	int m_waveFileSize;
	unsigned char* m_waveBuf;
	ALuint m_buffer;
};

#endif /* BUFFER_H_ */
