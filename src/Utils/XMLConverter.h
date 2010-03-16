/// @file
/// Set of functions for writing different values to XML.

#ifndef XMLConverter_h__
#define XMLConverter_h__

#include "Base.h"
#include "Array.h"

namespace Utils
{
	/// Set of functions for writing different values to XML.
	namespace XMLConverter
	{
		/// Converts the given arbitrary value to XML.
		template<typename T>
		void WriteToXML(ResourceSystem::XMLOutput& output, const T& val)
		{
			output.WriteString(StringConverter::ToString<T>(val));
		}

		/// Converts the given array of arbitrary values to XML.
		template<typename T>
		void WriteToXML(ResourceSystem::XMLOutput& output, Array<T>* array)
		{
			for (int32 i = 0; i < array->GetSize(); ++i)
			{
				output.BeginElement("Item");
				WriteToXML<T>(output, (*array)[i]);
				output.EndElement();
			}
		}

		template<>
		void WriteToXML(ResourceSystem::XMLOutput& output, const Vector2& val);

		template<>
		void WriteToXML(ResourceSystem::XMLOutput& output, const EntitySystem::EntityHandle& val);
	}
}

#endif