/*
 * buffer.cpp
 *
 *  Created on: June 21, 2016
 *      Author: Hotak
 */
#include "buffer.h"

Buffer::Buffer()
{
	m_waveBuf = NULL;
	m_waveFileSize = 0;
	m_buffer = 0;
}

Buffer::~Buffer()
{

}

void Buffer::Destroy()
{
	alDeleteBuffers(1, &m_buffer);
	delete m_waveBuf;
	m_waveBuf = NULL;
}

int Buffer::GetDataSize()
{
	return m_waveFileSize;
}

int Buffer::getFormat(short numChannels, short bitsPerSample)
{
	int format = 0;
	if (numChannels == 1) {
		if (bitsPerSample == 8 ) format = AL_FORMAT_MONO8;
		else if (bitsPerSample == 16) format = AL_FORMAT_MONO16;
	} else if (numChannels == 2) {
		if (bitsPerSample == 8 ) format = AL_FORMAT_STEREO8;
		else if (bitsPerSample == 16) format = AL_FORMAT_STEREO16;
	}
	return format;
}

void Buffer::readRiffHeader(FILE* waveFile)
{
	struct RIFF_Header riffHeader;
	fread(&riffHeader, sizeof(struct RIFF_Header), 1, waveFile);
	if ((riffHeader.chunkID[0] != 'R' ||
		riffHeader.chunkID[1] != 'I' ||
		riffHeader.chunkID[2] != 'F' ||
		riffHeader.chunkID[3] != 'F') ||
		(riffHeader.format[0] != 'W' ||
		riffHeader.format[1] != 'A' ||
		riffHeader.format[2] != 'V' ||
		riffHeader.format[3] != 'E'))
	{
		throw std::runtime_error("Invalid RIFF or WAVE Header");
	}
}

int Buffer::readWaveFormat(SUB_FORMAT_INFO* formatInfo, FILE* waveFile)
{
	struct WAVE_Format waveFormat;
	fread(&waveFormat, sizeof(struct WAVE_Format), 1, waveFile);
	if (waveFormat.subChunkID[0] != 'f' ||
		waveFormat.subChunkID[1] != 'm' ||
		waveFormat.subChunkID[2] != 't' ||
		waveFormat.subChunkID[3] != ' ')
	{
		throw std::runtime_error("Invalid Wave Format");
	}
	formatInfo->sampleRate = waveFormat.sampleRate;
	formatInfo->format = getFormat(waveFormat.numChannels, waveFormat.bitsPerSample);
	return waveFormat.subChunkSize;
}

void Buffer::readWaveDataInfo(int subChunkSize, FILE* waveFile)
{
	struct WAVE_Data waveData;
	std::string msg;
	if (subChunkSize > 16)
		fseek(waveFile, sizeof(short), SEEK_CUR);
	fread(&waveData, sizeof(struct WAVE_Data), 1, waveFile);
	if (waveData.subChunkID[0] != 'd' ||
		waveData.subChunkID[1] != 'a' ||
		waveData.subChunkID[2] != 't' ||
		waveData.subChunkID[3] != 'a')
	{
		throw std::runtime_error("Invalid data header");
	}
	m_waveFileSize = waveData.subChunk2Size;
}

void Buffer::readWaveData(void* buf, FILE* waveFile)
{
	fread(buf, m_waveFileSize, 1, waveFile);
}

bool Buffer::parseWave(SUB_FORMAT_INFO* formatInfo, std::string waveFilePath)
{
	try
	{
		FILE* waveFile = fopen(waveFilePath.c_str(), "rb");
		if (waveFile == NULL)
		{
			std::string msg = "fail to open file ";
			msg += waveFilePath;
			throw std::runtime_error(msg);
		}
		readRiffHeader(waveFile);
		int subChunkSize = readWaveFormat(formatInfo, waveFile);
		readWaveDataInfo(subChunkSize, waveFile);

		m_waveBuf = (void*)new char[m_waveFileSize];
		readWaveData(m_waveBuf, waveFile);
		fclose(waveFile);
	}
	catch(const std::runtime_error& e)
	{
		std::string msg = "fail to parse wave because ";
		msg += e.what();
		dlog_print(DLOG_FATAL, "Buffer::parseWave", msg.c_str());
		return false;
	}
	return true;
}

bool Buffer::GenerateBuffer(std::string waveFilePath)
{
	SUB_FORMAT_INFO formatInfo = {0, 0};
	if(!parseWave(&formatInfo, waveFilePath))
	{
		std::string msg = "audio file is not good.";
		dlog_print(DLOG_FATAL, "Buffer", msg.c_str());
		return false;
	}
	ALuint buffer;
	alGenBuffers(1, &buffer);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "GenerateBuffer", "error");
		return false;
	}
	m_buffer = buffer;
	alBufferData(m_buffer, formatInfo.format, m_waveBuf, m_waveFileSize, formatInfo.sampleRate);
	return true;
}

ALuint Buffer::GetBufferID()
{
	return m_buffer;
}
