/*
 * model.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <vector>
#include <string>

class Model{

public:
	Model();
	virtual ~Model();

	bool Create();
	void Destroy();

	//serialize
	bool Serialize();
	bool Load();


	void AddSrcToList(const char* srcFileName);
	const char* GetSrcFromList(int index);
	int GetSizeOfList();


protected:
	virtual bool creatspecifics() = 0;
	virtual void destroyspecifics() = 0;

private:
	bool m_created;
	std::vector<std::string> m_SrcList;

};



#endif /* MODEL_H_ */
