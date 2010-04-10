/// @file
/// Set of functions for converting different values to and from a string.

#ifndef StringConverter_h__
#define StringConverter_h__

#include "Base.h"
#include "Array.h"

// We are using the STD library for the conversion.
#include <sstream>

namespace Utils
{
	/// Set of functions for converting different values to and from a string.
	namespace StringConverter
	{
		/// Converts the given arbitrary value to a string.
		template<typename T>
		string ToString(const T& val)
		{
			std::ostringstream out;
			out << val;
			return out.str();
		}

		/// Converts the given array of arbitrary values to a string.
		template<typename T>
		string ToString(Array<T>* array)
		{
			std::ostringstream out;
			out << "[";
			for (int32 i = 0; i < array->GetSize(); ++i)
			{
				out << ToString<T>((*array)[i]);
				if (i != array->GetSize() - 1)
					out << ",";
			}
			out << "]";
			return out.str();
		}

		/// Converts the given bool value to a string.
		template<>
		string ToString(const bool& val);

		template<>
		inline string ToString(const string& val) { return val; }

		template<>
		string ToString(const Vector2& val);

		template<>
		string ToString(const GfxSystem::Color& val);

		template<>
		string ToString(const ResourceSystem::ResourcePtr& val);

		template<>
		string ToString(const PropertyFunctionParameters& val);

		template<>
		string ToString(const EntitySystem::EntityHandle& val);


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
		bool FromString(const string& str);

		template<>
		inline string FromString(const string& str) { return str; }

		template<>
		Vector2 FromString(const string& str);

		template<>
		GfxSystem::Color FromString(const string& str);

		template<>
		PropertyFunctionParameters FromString(const string& str);

		template<>
		EntitySystem::EntityHandle FromString(const string& str);

		template<>
		StringKey FromString(const string& str);

		template<>
		ResourceSystem::ResourcePtr FromString(const string& str);

		template<>
		PhysicalShape* FromString(const string& str);

		template<>
		PhysicalBody* FromString(const string& str);

	}
}

#endif // StringConverter_h__