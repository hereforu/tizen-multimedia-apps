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
