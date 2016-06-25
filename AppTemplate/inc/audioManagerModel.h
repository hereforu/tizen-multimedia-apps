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


typedef struct _ALObject{
	_ALObject()
	:source(NULL), buffer(NULL)
	{

	}
	Source* source;
	Buffer* buffer;
}ALObject;

typedef std::map<int, ALObject> ALObjectMap;

class AudioManagerModel : public Model
{
public:
	AudioManagerModel();
	~AudioManagerModel();

	unsigned int GetNumAllMediaItems();
	MediaContentItem& GetMediaInfo(int index);

	void GetSelectedSourceIdx(std::vector<unsigned int>& selectedsourceindex);
	bool IsAlreadySelected(int index);
	void UpdateSource(const std::vector<unsigned int>& selectedsourceindex);

	// play one source
	void PlaySource(unsigned int index);

	void PlaySources();
	void StopSources();

	//push, pop in context for play sources which are in context
	void PushSource(unsigned int index);
	void PopSource(unsigned int index);
	void ResetSource();

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
	void createSources(const std::vector<unsigned int>& selectedsourceindex);
	ALObject getobjectbyindex(int index);

private:
	Context m_context;
	ALObjectMap m_objmap;
	std::vector<MediaContentItem> m_audioList;


};

#endif /* AUDIOMANAGERMODEL_H_ */
