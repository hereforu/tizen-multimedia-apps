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

	template<typename TClass>
	class ObjectFactory
	{
	public:
		static TClass* CreateInstance()
		{
			TClass* pobject = new TClass;
			return pobject;
		}
	};


	template <typename Type>
	std::string ToString(Type val)
	{
	    std::stringstream stream;
	    stream << val;
	    return stream.str();
	}

	void Assert(bool condition);

	void Iferror_throw(int ret, const char* msg);
}

#endif /* BASE_H_ */
