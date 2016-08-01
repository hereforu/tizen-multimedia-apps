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

	void Create();
	void Destroy();

	//serialize
	bool Serialize();
	bool Load();

protected:
	virtual void creatspecifics() = 0;
	virtual void destroyspecifics() = 0;

private:
};



#endif /* MODEL_H_ */
