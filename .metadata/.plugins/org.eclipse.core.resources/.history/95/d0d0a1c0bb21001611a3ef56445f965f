/*
 * model.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef MODEL_H_
#define MODEL_H_


class Model{

public:
	Model();
	virtual ~Model();

	bool Create();
	void Destroy();

	//serialize
	bool Serialize();
	bool Load();


protected:
	virtual bool creatspecificmodel() = 0;
	virtual void destroyspecificmodel() = 0;

private:
	bool m_created;

};



#endif /* MODEL_H_ */
