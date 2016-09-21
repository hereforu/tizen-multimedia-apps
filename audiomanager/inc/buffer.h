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


/*
 * The Buffer class creates a Buffer object by reading in PCM data from a given .WAV file.
 */

#define AL_FORMAT_MONO8                           0x1100
#define AL_FORMAT_MONO16                          0x1101
#define AL_FORMAT_STEREO8                         0x1102
#define AL_FORMAT_STEREO16                        0x1103

/*
 * To understand these structures deeply, it is crucial to see the format of WAV file in advance.
 * Please refer to the canonical WAVE file format in http://soundfile.sapp.org/doc/WaveFormat.
 */

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
	bool GenerateBuffer(const char* wavefilepath);
	void Destroy();
	int GetDataSize();
	int GetFormat(short numChannels, short bitsPerSample);
	ALuint GetBufferID();

protected:

private:
	bool readPCM(FILE* fp, int PCMsize, unsigned char** buf);
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
