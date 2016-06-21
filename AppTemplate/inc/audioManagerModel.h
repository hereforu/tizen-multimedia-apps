/*
 * audioManagerModel.h
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */

#ifndef AUDIOMANAGERMODEL_H_
#define AUDIOMANAGERMODEL_H_

#define MAXNUM 5
#include "model.h"
#include "context.h"
#include "source.h"
#include "buffer.h"
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>

typedef std::vector<std::string> AudioPathVector;
typedef std::vector<unsigned int> SelectedSourceIdxVec;

struct OBJECT {
	Source source;
	Buffer buffer;
};

struct OBJECTVEC {
	std::vector<OBJECT> objVec;
	std::map<unsigned int, unsigned int> indexMap;//for find source position
};

class AudioManagerModel : public Model
{
public:
	AudioManagerModel();
	~AudioManagerModel();

	AudioPathVector GetAudioListinDB();
	SelectedSourceIdxVec GetSelectedSourceIdx();
	void UpdateSource(std::vector<unsigned int> index);
	void PlaySources();
	void StopSources();
	void LocateSource(unsigned int index, int x, int y, int z);
	void LocateListener(int x, int y, int z);

protected:
	bool creatspecifics();
	void destroyspecifics();

private:
	Context m_context;
	OBJECTVEC m_obj;
	AudioPathVector m_audioList;
	const ALCchar * m_defaultDeviceName;
	ALCdevice * m_device;
	ALCcontext* m_contextID;
};

#endif /* AUDIOMANAGERMODEL_H_ */
