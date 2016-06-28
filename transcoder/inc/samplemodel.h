/*
 * samplemodel.h
 *
 *  Created on: May 31, 2016
 *      Author: Jason
 */

#ifndef SAMPLEMODEL_H_
#define SAMPLEMODEL_H_

#include "model.h"

class SampleModel : public Model
{
public:
	SampleModel();
	~SampleModel();

protected:
	bool creatspecifics();
	void destroyspecifics();

};


#endif /* SAMPLEMODEL_H_ */
