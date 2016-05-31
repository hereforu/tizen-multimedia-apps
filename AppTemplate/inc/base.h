/*
 * base.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef BASE_H_
#define BASE_H_


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




	void Assert(bool condition);
}

#endif /* BASE_H_ */
