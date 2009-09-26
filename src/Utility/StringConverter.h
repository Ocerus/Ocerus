#ifndef StringConverter_h__
#define StringConverter_h__

#include "Settings.h"
#include "../GfxSystem/GfxStructures.h"
#include <sstream>

/** Set of functions for converting different values to and from a string.
*/
namespace StringConverter
{
	template<typename T>
	String ToString(T val)
	{
		std::ostringstream out;
		out << val;
		return out.str();
	}

	template<typename T>
	T FromString(const String& str)
	{
		T result;
		std::istringstream iss(str);
		if ((iss >> result).fail())
			return T(); // return default value
		return result;
	}

	template<>
	Vector2 FromString(const String& str);

	template<>
	GfxSystem::Color FromString(const String& str);

	template<>
	bool FromString(const String& str);

}

#endif // StringConverter_h__