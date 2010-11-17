/// @file
/// Set of functions for reading/writing different values from/to XML.

#ifndef XMLConverter_h__
#define XMLConverter_h__

#include "Base.h"
#include "Array.h"
#include "../ResourceSystem/XMLResource.h"

namespace Utils
{
	/// Set of functions for reading/writing different values from/to XML.
	namespace XMLConverter
	{
		/// Writes the given arbitrary value to XML.
		template<typename T>
		void WriteToXML(ResourceSystem::XMLOutput& output, const T& val)
		{
			output.WriteString(StringConverter::ToString<T>(val));
		}

		/// Writes the given array of arbitrary values to XML.
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

		template<>
		void WriteToXML(ResourceSystem::XMLOutput& output, const ResourceSystem::ResourcePtr& val);

		
		/// Reads the value from XML and returns it.
		template<typename T>
		T ReadFromXML(ResourceSystem::XMLNodeIterator& input)
		{
			return input.GetChildValue<T>();
		}

		template<>
		Vector2 ReadFromXML(ResourceSystem::XMLNodeIterator& input);

		template<>
		EntitySystem::EntityHandle ReadFromXML(ResourceSystem::XMLNodeIterator& input);

		template<>
		ResourceSystem::ResourcePtr ReadFromXML(ResourceSystem::XMLNodeIterator& input);
	}
}

#endif