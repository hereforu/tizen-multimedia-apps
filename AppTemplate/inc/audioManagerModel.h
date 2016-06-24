/*
 * audioManagerModel.h
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */

#ifndef AUDIOMANAGERMODEL_H_
#define AUDIOMANAGERMODEL_H_

#define MAXNUM 5
#define NORMALIZE_X 8
#define NORMALIZE_Z 6

#include "model.h"
#include "context.h"
#include "source.h"
#include "buffer.h"
#include "mediacontentcontroller.h"
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>

//typedef std::vector<std::string> AudioPath;
typedef std::vector<MediaContentItem> AudioPathVector;
typedef std::vector<unsigned int> SelectedSourceIdxVec;

struct OBJECT {
	Source* source;
	Buffer* buffer;
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

	AudioPathVector GetAudioList();
	SelectedSourceIdxVec GetSelectedSourceIdx();
	void UpdateSource(std::vector<unsigned int> selectedSourceIdx);

	// play one source
	void PlaySource(unsigned int index);

	void PlaySources();
	void StopSources();

	//push, pop in context for play sources which are in context
	void PushSource(unsigned int index);
	void PopSource(unsigned int index);

	/*
	OpenAL uses a right-handed Cartesian coordinate system (RHS),
	where in a frontal default view X (thumb) points right,
	Y (index finger) points up, and Z (middle finger) points towards the viewer/camera.
	*/
	//you put pixel value. func change pixel -> openal value(normalize)
	void LocateSource(unsigned int index, int x, int y, int z);
	void LocateListener(int x, int y, int z);

protected:
	bool creatspecifics();
	void destroyspecifics();

private:
	void getAudioListinDB();
	void removeAllSources();
	void createSources(std::vector<unsigned int> selectedSourceIdx);
	Context m_context;
	OBJECTVEC m_obj;
	AudioPathVector m_audioList;

	ALCdevice * m_device;
	ALCcontext* m_contextID;
};

#endif /* AUDIOMANAGERMODEL_H_ */
