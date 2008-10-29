#ifndef StringConverter_h__
#define StringConverter_h__

#include "Settings.h"
#include <sstream>

namespace StringConverter
{
	template<typename T>
	string ToString(T val)
	{
		std::ostringstream out;
		out << val;
		return out.str();
	}

	template<typename T>
	T FromString(const string& str)
	{
		T result;
		std::istringstream iss(str);
		if (!(iss >> result).fail())
			return T(); // return default value
		return result;
	}

}

#endif // StringConverter_h__