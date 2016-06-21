/*
 * audioManagerModel.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "audioManagerModel.h"
#include "mediacontentcontroller.h"
#include <app_common.h>//
AudioManagerModel::AudioManagerModel()
{
	m_defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	m_device = alcOpenDevice(m_defaultDeviceName);
	m_contextID = alcCreateContext(m_device, NULL);
	alcMakeContextCurrent(m_contextID);
	m_context = Context();
	GetAudioListinDB();
}

AudioManagerModel::~AudioManagerModel()
{
	for(int i = 0 ; i < m_obj.objVec.size() ; i++)
	{
		m_obj.objVec[i].source.Destroy();
		m_obj.objVec[i].buffer.Destroy();
	}
	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_contextID);
	alcCloseDevice(m_device);
}

void AudioManagerModel::GetAudioListinDB()
{
	//MediaContentController::getContentsByCondition(m_audioList);
	char* resource_path = app_get_resource_path();
	std::string path = resource_path;

	std::string wav_file_name = "1.wav";
	path = path + wav_file_name;
	m_audioList.push_back(path);
	path = resource_path;

	wav_file_name = "2.wav";
	path = path + wav_file_name;
	m_audioList.push_back(path);
	path = resource_path;

	wav_file_name = "3.wav";
	path = path + wav_file_name;
	m_audioList.push_back(path);
	path = resource_path;

	wav_file_name = "five.wav";
	path = path + wav_file_name;
	m_audioList.push_back(path);
	path = resource_path;

	wav_file_name = "four.wav";
	path = path + wav_file_name;
	m_audioList.push_back(path);
	path = resource_path;

	wav_file_name = "seven.wav";
	path = path + wav_file_name;
	m_audioList.push_back(path);
	path = resource_path;

	wav_file_name = "six.wav";
	path = path + wav_file_name;
	m_audioList.push_back(path);
}
AudioPathVector AudioManagerModel::GetAudioList()
{
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

void AudioManagerModel::LocateSource(unsigned int index, int x, int y, int z)
{
	unsigned int pos = m_obj.indexMap.find(index)->second;
	if(pos == MAXNUM) return;
	m_context.setSourcePos(m_obj.objVec[pos].source.GetSourceId(), x, y, z);
}

void AudioManagerModel::LocateListener(int x, int y, int z)
{
	m_context.setListenerPos(x, y, z);
}

SelectedSourceIdxVec AudioManagerModel::GetSelectedSourceIdx()
{
	SelectedSourceIdxVec ret;
	map<unsigned int, unsigned int>::iterator i;
	for(i = m_obj.indexMap.begin() ; i != m_obj.indexMap.end() ; i++)
	{
		ret.push_back(i->first);
	}
	return ret;
}

bool AudioManagerModel::creatspecifics()
{
	return true;
}

void AudioManagerModel::destroyspecifics()
{

}
