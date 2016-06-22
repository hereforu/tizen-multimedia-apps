/*
 * buffer.cpp
 *
 *  Created on: June 21, 2016
 *      Author: Hotak
 */
#include "buffer.h"

Buffer::Buffer()
{
	m_waveFile = NULL;
	m_waveBuf = NULL;
	m_buffer = NULL;
}

Buffer::~Buffer()
{

}

void Buffer::Destroy()
{
	delete m_waveBuf;
	m_waveBuf = NULL;
}

int Buffer::GetDataSize()
{
	return m_waveData.subChunk2Size;
}

int Buffer::GetFrequency()
{
	return m_waveFormat.sampleRate;
}

int Buffer::GetFormat()
{
	int format = 0;
	if (m_waveFormat.numChannels == 1) {
		if (m_waveFormat.bitsPerSample == 8 ) format = AL_FORMAT_MONO8;
		else if (m_waveFormat.bitsPerSample == 16) format = AL_FORMAT_MONO16;
	} else if (m_waveFormat.numChannels == 2) {
		if (m_waveFormat.bitsPerSample == 8 ) format = AL_FORMAT_STEREO8;
		else if (m_waveFormat.bitsPerSample == 16) format = AL_FORMAT_STEREO16;
	}
	return format;
}

void Buffer::OpenFile(std::string pWavFilePath)
{
	m_waveFile = fopen(pWavFilePath.c_str(), "rb");
	if (m_waveFile == NULL)
	{
		std::string msg = "fail to open file ";
		msg += pWavFilePath;
		throw msg;
	}
}

void Buffer::ReadRiffHeader()
{
	std::string msg;
	fread(&m_riffHeader, sizeof(struct RIFF_Header), 1, m_waveFile);
	if ((m_riffHeader.chunkID[0] != 'R' ||
		 m_riffHeader.chunkID[1] != 'I' ||
		 m_riffHeader.chunkID[2] != 'F' ||
		 m_riffHeader.chunkID[3] != 'F') ||
		(m_riffHeader.format[0] != 'W' ||
		 m_riffHeader.format[1] != 'A' ||
		 m_riffHeader.format[2] != 'V' ||
		 m_riffHeader.format[3] != 'E'))
	{
		msg = "Invalid RIFF or WAVE Header";
		throw msg;
	}
}

void Buffer::ReadWaveFormat()
{
	std::string msg;
	fread(&m_waveFormat, sizeof(struct WAVE_Format), 1, m_waveFile);
	if (m_waveFormat.subChunkID[0] != 'f' ||
		m_waveFormat.subChunkID[1] != 'm' ||
		m_waveFormat.subChunkID[2] != 't' ||
		m_waveFormat.subChunkID[3] != ' ')
	{
		msg = "Invalid Wave Format";
		throw msg;
	}
}

void Buffer::ReadWaveDataInfo()
{
	std::string msg;
	if (m_waveFormat.subChunkSize > 16)
		fseek(m_waveFile, sizeof(short), SEEK_CUR);
	fread(&m_waveData, sizeof(struct WAVE_Data), 1, m_waveFile);
	if (m_waveData.subChunkID[0] != 'd' ||
		m_waveData.subChunkID[1] != 'a' ||
		m_waveData.subChunkID[2] != 't' ||
		m_waveData.subChunkID[3] != 'a')
	{
		msg = "Invalid data header";
		throw msg;
	}
}

void Buffer::ReadWaveData(void* buf)
{
	fread(buf, m_waveData.subChunk2Size, 1, m_waveFile);
}

bool Buffer::ParseWave(std::string waveFilePath)
{
	try
	{
		OpenFile(waveFilePath);
		ReadRiffHeader();
		ReadWaveFormat();
		ReadWaveDataInfo();

		m_waveBuf = (void*)new char[GetDataSize()];
		ReadWaveData(m_waveBuf);
		if(m_waveFile != NULL)
			fclose(m_waveFile);
	}
	catch(std::string msg)
	{
		dlog_print(DLOG_FATAL, "Buffer", msg.c_str());
		return false;
	}
	return true;
}
bool Buffer::GenBuffer(std::string waveFilePath)
{
	if(!ParseWave(waveFilePath))
	{
		std::string msg = "audio file is not good.";
		dlog_print(DLOG_FATAL, "Buffer", msg.c_str());
		return false;
	}
	alGenBuffers(1, &m_buffer);
	alBufferData(m_buffer, GetFormat(), m_waveBuf, GetDataSize(), GetFrequency());
	return true;
}

ALuint Buffer::GetBufferID()
{
	return m_buffer;
}
