#ifndef StringConverter_h__
#define StringConverter_h__

#include "Settings.h"
#include <sstream>

namespace StringConverter
{
	string NumToStr(uint64 val)
	{
		std::stringstream out;
		out << val;
		return out.str();
	}


}

#endif // StringConverter_h__