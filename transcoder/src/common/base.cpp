/*
 * base.cpp

 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include <assert.h>
#include <stdexcept>

void AppTool::assert_ifnot(bool condition)
{
	assert(condition);
}

void AppTool::iferror_throw(int ret, const char* msg)
{
	if(ret != 0)
		throw std::runtime_error(std::string(msg)+to_string(ret));
}



