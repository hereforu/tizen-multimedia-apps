/*
 * buffer.cpp
 *
 *  Created on: June 21, 2016
 *      Author: Hotak
 */
#include "common/base.h"
#include "buffer.h"
#include <stdexcept>
Buffer::Buffer()
:m_waveBuf(NULL), m_waveFileSize(0), m_buffer(0)
{

}

Buffer::~Buffer()
{
	Destroy();
}

void Buffer::Destroy()
{
	release_resources();
}

int Buffer::GetDataSize()
{
	return m_waveFileSize;
}

void Buffer::release_resources()
{
	if(m_buffer != 0)
	{
		alDeleteBuffers(1, &m_buffer);
		m_buffer = 0;
	}
	SAFE_ARRAY_DELETE(m_waveBuf);
}
//stereo sample does not support spatial differences
int Buffer::getFormat(short numChannels, short bitsPerSample)
{
	int format = 0;
	if (numChannels == 1)
	{
		if (bitsPerSample == 8 )
		{
			format = AL_FORMAT_MONO8;
			dlog_print(DLOG_DEBUG, "Buffer", "AL_FORMAT_MONO8");
		}
		else if (bitsPerSample == 16)
		{
			format = AL_FORMAT_MONO16;
			dlog_print(DLOG_DEBUG, "Buffer", "AL_FORMAT_MONO16");
		}
	}
	else if (numChannels == 2)
	{
		if (bitsPerSample == 8 )
		{
			format = AL_FORMAT_STEREO8;
			dlog_print(DLOG_DEBUG, "Buffer", "AL_FORMAT_STEREO8");
		}
		else if (bitsPerSample == 16)
		{
			format = AL_FORMAT_STEREO16;
			dlog_print(DLOG_DEBUG, "Buffer", "AL_FORMAT_STEREO16");
		}
	}
	return format;
}

bool Buffer::readRiffHeader(FILE* waveFile)
{
	struct RIFF_Header riffHeader;
	if(fread(&riffHeader, sizeof(struct RIFF_Header), 1, waveFile)==1)
	{
		if ((riffHeader.chunkID[0] == 'R' &&
			riffHeader.chunkID[1] == 'I' &&
			riffHeader.chunkID[2] == 'F' &&
			riffHeader.chunkID[3] == 'F') &&
			(riffHeader.format[0] == 'W' &&
			riffHeader.format[1] == 'A' &&
			riffHeader.format[2] == 'V' &&
			riffHeader.format[3] == 'E'))
		{
			return true;
		}
	}
	dlog_print(DLOG_ERROR, "Buffer", "fail to readRiffHeader");
	return false;
}

int Buffer::readwaveformat_and_get_chunksize(SUB_FORMAT_INFO* formatInfo, FILE* waveFile)
{
	struct WAVE_Format waveFormat;
	if(fread(&waveFormat, sizeof(struct WAVE_Format), 1, waveFile)==1)
	{
		if (waveFormat.subChunkID[0] == 'f' &&
			waveFormat.subChunkID[1] == 'm' &&
			waveFormat.subChunkID[2] == 't' &&
			waveFormat.subChunkID[3] == ' ')
		{
			formatInfo->sampleRate = waveFormat.sampleRate;
			formatInfo->format = getFormat(waveFormat.numChannels, waveFormat.bitsPerSample);
			return waveFormat.subChunkSize;
		}
	}
	dlog_print(DLOG_ERROR, "Buffer", "fail to readWaveFormat");
	return 0;
}

int Buffer::readwavedatainfo_and_get_wavefilesize(int subChunkSize, FILE* waveFile)
{
	struct WAVE_Data waveData;
	if (subChunkSize > 16)
	{
		fseek(waveFile, sizeof(short), SEEK_CUR);
	}
	if(fread(&waveData, sizeof(struct WAVE_Data), 1, waveFile)==1)
	{
		if (waveData.subChunkID[0] == 'd' &&
			waveData.subChunkID[1] == 'a' &&
			waveData.subChunkID[2] == 't' &&
			waveData.subChunkID[3] == 'a')
		{
			return waveData.subChunk2Size;
		}
	}
	dlog_print(DLOG_ERROR, "Buffer", "fail to readWaveDataInfo");
	return 0;
}


bool Buffer::parseWave(SUB_FORMAT_INFO* formatInfo, const char* wavefilepath)
{
	FILE* fp = NULL;
	do
	{
		FILE* fp = NULL;
		if ((fp = fopen(wavefilepath, "rb")) == NULL)
			break;
		if(readRiffHeader(fp) == false)
			break;
		if((m_waveFileSize = readwavedatainfo_and_get_wavefilesize(readwaveformat_and_get_chunksize(formatInfo, fp), fp))==0)
			break;
		m_waveBuf = new unsigned char[m_waveFileSize];
		if(fread(m_waveBuf, m_waveFileSize, 1, fp)!= 1)
		{
			SAFE_ARRAY_DELETE(m_waveBuf);
			break;
		}
		return true;
	}while(0);

	if(fp)
		fclose(fp);
	dlog_print(DLOG_ERROR, "Buffer", "fail to parse %s", wavefilepath);
	return false;
}

bool Buffer::GenerateBuffer(const char* wavefilepath)
{
	SUB_FORMAT_INFO formatInfo = {0, 0};
	if(!parseWave(&formatInfo, wavefilepath))
	{
		return false;
	}

	ALenum ret = 0;
	alGenBuffers(1, &m_buffer);
	if ((ret=alGetError()) != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "Buffer", "alGenBuffers error:%d", ret);
		release_resources();
		return false;
	}

	alBufferData(m_buffer, formatInfo.format, m_waveBuf, m_waveFileSize, formatInfo.sampleRate);
	if ((ret=alGetError()) != AL_NO_ERROR)
	{
		dlog_print(DLOG_ERROR, "Buffer", "alBufferData error:%d", ret);
		release_resources();
		return false;
	}
	return true;
}

ALuint Buffer::GetBufferID()
{
	return m_buffer;
}
