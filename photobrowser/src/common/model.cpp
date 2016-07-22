/*
 * model.cpp

 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "common/model.h"
#include <assert.h>
#include <stdexcept>

Model::Model()
{
}

Model::~Model()
{
}

void Model::Create()
{
	try
	{
		creatspecifics();
	}
	catch(const std::runtime_error& e)
	{
		//how to delete evas_object ?
		std::string msg = "fail to create model because ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Model::Destroy()
{
	destroyspecifics();
}

//serialize
bool Model::Serialize()
{
	return true;
}

bool Model::Load()
{
	return true;
}

