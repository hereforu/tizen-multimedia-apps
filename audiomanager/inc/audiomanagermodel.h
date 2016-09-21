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

#include "common/model.h"
#include "context.h"
#include "source.h"
#include "buffer.h"
#include "common/mediacontentcontroller.h"
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>


/*
 * Basically, the relation between Source and Buffer is many to one in OpenAL.
 * But, in this application, one Buffer is going to be assigned to only one Source
 * i.e. the relation between two objects is one to one
 */
typedef struct _ALObject{
	_ALObject()
	:source(NULL), buffer(NULL)
	{}
	Source* source;
	Buffer* buffer;
}ALObject;

/*
 * the key of the ALObjectMap is source index
 */
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
	void LocateSource(unsigned int index, float x, float y, float z);
	void LocateListener(float x, float y, float z);

protected:
	void creatspecifics();
	void destroyspecifics();

private:
	void getaudiolistinDB();
	void removeallsources();
	void createsources(const std::vector<unsigned int>& selectedsourceindex);
	ALObject getobjectbyindex(int index);

private:
	Context m_context;
	ALObjectMap m_objmap;
	std::vector<MediaContentItem> m_audiolist;
};

#endif /* AUDIOMANAGERMODEL_H_ */
