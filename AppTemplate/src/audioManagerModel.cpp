/*
 * audioManagerModel.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "audioManagerModel.h"
#include <app_common.h>//
AudioManagerModel::AudioManagerModel()
{
	const ALCchar * m_defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	m_device = alcOpenDevice(m_defaultDeviceName);
	m_contextID = alcCreateContext(m_device, NULL);
	alcMakeContextCurrent(m_contextID);
	m_context = Context();
	getAudioListinDB();
}

AudioManagerModel::~AudioManagerModel()
{
	for(int i = 0 ; i < m_obj.objVec.size() ; i++)
	{
		m_obj.objVec[i].source->Destroy();
		delete m_obj.objVec[i].source;
		m_obj.objVec[i].source = NULL;
		m_obj.objVec[i].buffer->Destroy();
		delete m_obj.objVec[i].buffer;
		m_obj.objVec[i].buffer = NULL;
	}
	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_contextID);
	alcCloseDevice(m_device);
}

void AudioManagerModel::getAudioListinDB()
{
	MediaContent mediaContent;
	MediaContentParam param;
	param.mediatype = MC_SOUND_TYPE;
	if(!mediaContent.IsConnected())
		mediaContent.ConnectDB();
	mediaContent.GetItem(param, &m_audioList);
	mediaContent.DisconnectDB();
//	char* resource_path = app_get_resource_path();
//	std::string path = resource_path;
//
//	std::string wav_file_name = "1.wav";
//	path = path + wav_file_name;
//	m_audioList.push_back(path);
//	path = resource_path;
//
//	wav_file_name = "2.wav";
//	path = path + wav_file_name;
//	m_audioList.push_back(path);
//	path = resource_path;
//
//	wav_file_name = "3.wav";
//	path = path + wav_file_name;
//	m_audioList.push_back(path);
//	path = resource_path;
//
//	wav_file_name = "five.wav";
//	path = path + wav_file_name;
//	m_audioList.push_back(path);
//	path = resource_path;
//
//	wav_file_name = "four.wav";
//	path = path + wav_file_name;
//	m_audioList.push_back(path);
//	path = resource_path;
//
//	wav_file_name = "seven.wav";
//	path = path + wav_file_name;
//	m_audioList.push_back(path);
//	path = resource_path;
//
//	wav_file_name = "six.wav";
//	path = path + wav_file_name;
//	m_audioList.push_back(path);
}
AudioPathVector AudioManagerModel::GetAudioList()
{
	return m_audioList;
}

void AudioManagerModel::removeAllSources()
{
	for(int i = 0 ; i < m_obj.objVec.size() ; i++)
	{
		m_obj.objVec[i].source->Destroy();
		delete m_obj.objVec[i].source;
		m_obj.objVec[i].source = NULL;
		m_obj.objVec[i].buffer->Destroy();
		delete m_obj.objVec[i].buffer;
		m_obj.objVec[i].buffer = NULL;
	}
	m_obj.objVec.clear();
	m_obj.indexMap.clear();
}

void AudioManagerModel::createSources(std::vector<unsigned int> selectedSourceIdx)
{
	for(int i = 0 ; i < selectedSourceIdx.size() ; i++)
	{
		OBJECT object;
		object.buffer = new Buffer();
		object.source = new Source();
		if(!object.buffer->GenerateBuffer(m_audioList[selectedSourceIdx[i]].path))
		{
			std::string msg = m_audioList[selectedSourceIdx[i]].path + "   <error>";
			dlog_print(DLOG_FATAL, "AudioManagerModel", msg.c_str());
			continue;
		}
		object.source->GenerateSource(object.buffer->GetBufferID());
		m_obj.objVec.push_back(object);
		m_obj.indexMap.insert(std::pair<unsigned int, unsigned int>(selectedSourceIdx[i], m_obj.objVec.size()-1));
	}
}

void AudioManagerModel::UpdateSource(std::vector<unsigned int> selectedSourceIdx)
{
	removeAllSources();
	createSources(selectedSourceIdx);
	m_context.ResetSource();
}

void AudioManagerModel::PlaySources()
{
	m_context.Play();
}

void AudioManagerModel::PlaySource(unsigned int index)
{
	unsigned int pos = m_obj.indexMap.find(index)->second;
	if(pos == MAXNUM) return;
	m_obj.objVec[pos].source->Play();
}

void AudioManagerModel::StopSources()
{
	m_context.Stop();
}

void AudioManagerModel::LocateSource(unsigned int index, int x, int y, int z)
{
	unsigned int pos = m_obj.indexMap.find(index)->second;
	if(pos == MAXNUM) return;
	m_context.setSourcePos(m_obj.objVec[pos].source->GetSourceId(), NORMALIZE_X * x, y, NORMALIZE_Z * z);
}

void AudioManagerModel::LocateListener(int x, int y, int z)
{
	m_context.setListenerPos(NORMALIZE_X * x, y, NORMALIZE_Z * z);
}

void AudioManagerModel::PushSource(unsigned int index)
{
	unsigned int pos = m_obj.indexMap.find(index)->second;
	m_context.Push(m_obj.objVec[pos].source);
}

void AudioManagerModel::PopSource(unsigned int index)
{
	unsigned int pos = m_obj.indexMap.find(index)->second;
	m_context.Pop(m_obj.objVec[pos].source);
}

SelectedSourceIdxVec AudioManagerModel::GetSelectedSourceIdx()
{
	SelectedSourceIdxVec ret;
	std::map<unsigned int, unsigned int>::iterator i;
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
