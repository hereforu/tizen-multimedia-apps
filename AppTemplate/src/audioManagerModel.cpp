/*
 * audioManagerModel.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "audioManagerModel.h"
#include "mediacontentcontroller.h"

AudioManagerModel::AudioManagerModel()
{
	const ALCchar * defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	ALCdevice * device = alcOpenDevice(defaultDeviceName);
	ALCcontext* context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	m_context = Context();
}

AudioManagerModel::~AudioManagerModel()
{
	for(int i = 0 ; i < m_obj.objVec.size() ; i++)
	{
		m_obj.objVec[i].source.Destroy();
		m_obj.objVec[i].buffer.Destroy();
	}
}

AudioPathVector AudioManagerModel::GetAudioListinDB()
{
	MediaContentController::getContentFromDB(0, m_audioList);
	return m_audioList;
}

// listener index is zero
void AudioManagerModel::UpdateSource(std::vector<unsigned int> index)
{
	m_context.ResetSource();
	for(int i = 0 ; i < m_obj.objVec.size() ; i++)
	{
		m_obj.objVec[i].source.Destroy();
		m_obj.objVec[i].buffer.Destroy();
	}
	m_obj.indexMap.clear();
	for(int i = 0 ; i < index.size() ; i++)
	{
		OBJECT object;
		object.buffer = Buffer();
		object.source = Source();
		if(!object.buffer.GenBuffer(m_audioList[index[i] - 1]))
		{
			std::string msg = m_audioList[index[i] - 1] + "   <error>";
			dlog_print(DLOG_FATAL, "AudioManagerModel", msg.c_str());
			continue;
		}
		object.source.GenSource(object.buffer.GetBufferID());
		m_obj.objVec.push_back(object);
		m_obj.indexMap.insert(pair<unsigned int, unsigned int>(index[i], m_obj.objVec.size()-1));
	}
}

void AudioManagerModel::PlaySources()
{
	m_context.Play();
}

void AudioManagerModel::StopSources()
{
	m_context.Stop();
}

void AudioManagerModel::LocateSource(unsigned int index, float x, float y, float z)
{
	unsigned int pos = m_obj.indexMap.find(index)->second;
	if(pos == MAXNUM) return;
	m_context.setSourcePos(m_obj.objVec[pos].source.GetSourceId(), x, y, z);
}

void AudioManagerModel::LocateListener(float x, float y, float z)
{
	m_context.setListenerPos(x, y, z);
}
