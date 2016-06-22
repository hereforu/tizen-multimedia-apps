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

class Buffer
{
public:
	Buffer();
	~Buffer();
	void Destroy();
	int GetDataSize();
	int GetFrequency();
	int GetFormat();

	bool GenerateBuffer(std::string waveFilePath);
	ALuint GetBufferID();
protected:

private:
	void OpenFile(std::string pWavFilePath);
	void ReadRiffHeader();
	void ReadWaveFormat();
	void ReadWaveDataInfo();
	void ReadWaveData(void* buf);
	bool ParseWave(std::string waveFilePath);

	FILE* m_waveFile;
	struct RIFF_Header m_riffHeader;
	struct WAVE_Format m_waveFormat;
	struct WAVE_Data m_waveData;

	void* m_waveBuf;
	ALuint m_buffer;
};

#endif /* BUFFER_H_ */
