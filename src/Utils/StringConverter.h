#ifndef StringConverter_h__
#define StringConverter_h__

#include "Base.h"

// We are using the STD library for the conversion.
#include <sstream>

namespace Utils
{
	/// Set of functions for converting different values to and from a string.
	namespace StringConverter
	{
		/// Converts the given arbitrary value to a string.
		template<typename T>
		string ToString(T val)
		{
			std::ostringstream out;
			out << val;
			return out.str();
		}

		/// Template specialization.
		template<>
		inline string ToString(const string& val) { return val; }

		/// Template specialization.
		template<>
		inline string ToString(const string val) { return val; }

		template<>
		string ToString(bool& val);
		
		template<>
		string ToString(bool val);

#ifdef __WIN__
#pragma warning(push)
#pragma warning(disable: 4700)
#endif

		/// Converts the given string to a value of an arbitrary type.
		template<typename T>
		T FromString(const string& str)
		{
			T result;
			std::istringstream iss(str);
			if ((iss >> result).fail())
				return T(); // return default value
			return result;
		}

#ifdef __WIN__
#pragma warning(pop)
#endif

		template<>
		inline string FromString(const string& str) { return str; }

		template<>
		char* FromString(const string& str);

		template<>
		Vector2 FromString(const string& str);

		template<>
		GfxSystem::Color FromString(const string& str);

		template<>
		bool FromString(const string& str);

	}
}

#endif // StringConverter_h__