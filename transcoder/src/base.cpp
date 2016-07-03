/*
 * base.cpp

 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "base.h"
#include <assert.h>
#include <stdexcept>

void AppTool::Assert(bool condition)
{
	assert(condition);
}

void AppTool::Iferror_throw(int ret, const char* msg)
{
	if(ret != 0)
		throw std::runtime_error(std::string(msg)+ToString(ret));
}



