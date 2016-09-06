/*
 * base.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef BASE_H_
#define BASE_H_

#include "photobrowser.h" //auto generated header by IDE
#include <string>
#include <sstream>

#define SAFE_FREE(x)\
	if(x){\
		free(x);\
		x = NULL;\
	}\

#define SAFE_DELETE(x)\
	if(x){\
		delete x;\
		x = NULL;\
	}\

#define SAFE_ARRAY_DELETE(x)\
	if(x){\
		delete[] x;\
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
	std::string ToString(Type val)
	{
	    std::stringstream stream;
	    stream << val;
	    return stream.str();
	}

	void Assert(bool condition);
}

#endif /* BASE_H_ */
