/*
 * model.cpp

 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "base.h"
#include "model.h"
#include <assert.h>

Model::Model()
	:m_created(false)
{

}


Model::~Model()
{
	AppTool::Assert(m_created==false);
}

bool Model::Create()
{
	m_created = creatspecifics();
	m_created = true;
	return m_created;
}

void Model::Destroy()
{
	destroyspecifics();
	m_created = false;
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


