/*
 * base.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef BASE_H_
#define BASE_H_

#include "transcoder.h" //auto generated header by IDE
#include <string>
#include <sstream>

#define SAFE_DELETE(x)\
	if(x){\
		delete x;\
		x = NULL;\
	}\

#define SAFE_EVAS_DELETE(x)\
	if(x){\
		evas_object_del(x);\
		x = NULL;\
	}\

namespace AppTool
{
	template <typename Type>
	std::string to_string(Type val)
	{
	    std::stringstream stream;
	    stream << val;
	    return stream.str();
	}

	void assert_ifnot(bool condition);

	void iferror_throw(int ret, const char* msg);
}

using namespace AppTool;

#endif /* BASE_H_ */
