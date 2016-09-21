/*
 * audioManagerModel.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "common/base.h"
#include "audioManagerModel.h"
#include <app_common.h>


AudioManagerModel::AudioManagerModel()
{
	//TODO:: initialize variables
}

AudioManagerModel::~AudioManagerModel()
{

}

void AudioManagerModel::creatspecifics()
{
	getaudiolistinDB();
	m_context.Create();
}

void AudioManagerModel::destroyspecifics()
{
	m_context.Destroy();
	removeallsources();
	m_audiolist.clear();
}

unsigned int AudioManagerModel::GetNumAllMediaItems()
{
	return m_audiolist.size();
}

MediaContentItem& AudioManagerModel::GetMediaInfo(int index)
{
	AppTool::Assert( index >= 0 && index <  m_audiolist.size());
	return m_audiolist[index];
}

void AudioManagerModel::UpdateSource(const std::vector<unsigned int>& selectedsourceindex)
{
	removeallsources();
	createsources(selectedsourceindex);
	m_context.ResetSource();
}

void AudioManagerModel::PlaySources()
{
	m_context.Play();
}

bool AudioManagerModel::IsAlreadySelected(int index)
{
	return ( m_objmap.find(index) !=m_objmap.end() )?true:false;
}


void AudioManagerModel::PlaySource(unsigned int index)
{
	ALObject obj = getobjectbyindex(index);
	if(obj.source)
	{
		obj.source->Play();
	}
}

void AudioManagerModel::StopSources()
{
	m_context.Stop();
}

void AudioManagerModel::LocateSource(unsigned int index, float x, float y, float z)
{
	ALObject obj = getobjectbyindex(index);
	if(obj.source)
	{
		obj.source->SetSourcePos(x, y, z);
	}
}

void AudioManagerModel::LocateListener(float x, float y, float z)
{
	m_context.SetListenerPos(x, y, z);
}

void AudioManagerModel::ResetSource()
{
	m_context.ResetSource();
}
void AudioManagerModel::PushSource(unsigned int index)
{
	ALObject obj = getobjectbyindex(index);
	if(obj.source)
		m_context.Push(obj.source);
}

void AudioManagerModel::PopSource(unsigned int index)
{
	ALObject obj = getobjectbyindex(index);
	if(obj.source)
		m_context.Pop(obj.source);
}

void AudioManagerModel::GetSelectedSourceIdx(std::vector<unsigned int>& selectedsourceindex)
{
	ALObjectMap::iterator iter = m_objmap.begin();
	while(iter != m_objmap.end())
	{
		selectedsourceindex.push_back(iter->first);
		++iter;
	}
}
void AudioManagerModel::removeallsources()
{
	ALObjectMap::iterator iter = m_objmap.begin();
	while(iter != m_objmap.end())
	{
		ALObject obj = iter->second;

		obj.source->Destroy();
		delete obj.source;
		obj.buffer->Destroy();
		delete obj.buffer;
		++iter;
	}
	m_objmap.clear();
}

void AudioManagerModel::createsources(const std::vector<unsigned int>& selectedsourceindex)
{
	for(int i = 0 ; i < selectedsourceindex.size() ; i++)
	{
		ALObject object;

		object.buffer = new Buffer();
		if(!object.buffer->GenerateBuffer(m_audiolist[selectedsourceindex[i]].path.c_str()))
		{
			dlog_print(DLOG_ERROR, "AudioManagerModel", "fail to create the buffer of %s",  m_audiolist[selectedsourceindex[i]].path.c_str());
			delete object.buffer;
			continue;
		}
		object.source = new Source();
		if(!object.source->GenerateSource(object.buffer->GetBufferID()))
		{
			dlog_print(DLOG_ERROR, "AudioManagerModel", "fail to create the GenerateSource of %s",  m_audiolist[selectedsourceindex[i]].path.c_str());
			delete object.source;
			delete object.buffer;
			continue;
		}

		m_objmap[selectedsourceindex[i]]=object;
	}
}

void AudioManagerModel::getaudiolistinDB()
{
	MediaContent mediaContent;
	MediaContentParam param;
	param.mediatype = MC_SOUND_TYPE | MC_MUSIC_TYPE;
	if(!mediaContent.IsConnected())
	{
		mediaContent.ConnectDB();
	}
	mediaContent.GetItem(param, &m_audiolist);
	mediaContent.DisconnectDB();
}
ALObject AudioManagerModel::getobjectbyindex(int index)
{
	ALObjectMap::iterator iter = m_objmap.find(index);
	if(iter==m_objmap.end())
	{
		dlog_print(DLOG_FATAL, "AudioManagerModel", "fail play source(%d)", index);
		return ALObject();
	}
	return iter->second;
}


