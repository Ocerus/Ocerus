/// @file
/// Set of functions for writing different values to XML.

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

		
		/// Converts the given arbitrary value to XML.
		template<typename T>
		T ReadFromXML(ResourceSystem::XMLNodeIterator& input)
		{
			return input.GetChildValue<T>();
		}

		/// Converts the given array of arbitrary values to XML.
		/*template<typename U>
		Array<U>* ReadFromXML(ResourceSystem::XMLNodeIterator& input)
		{
			vector<U> vertices;
			for (ResourceSystem::XMLNodeIterator vertIt = input.IterateChildren(); vertIt != input.EndChildren(); ++vertIt)
			{
				if ((*vertIt).compare("Item") == 0) { vertices.push_back(vertIt.GetChildValue<U>()); }
				else ocError << "XML:Entity: Expected 'Item', found '" << *vertIt << "'";
			}

			Array<U> vertArray(vertices.size());
			for (uint32 i=0; i<vertices.size(); ++i)
			{
				vertArray[i] = vertices[i];
			}
			return &vertArray; //!!!
		}*/

		template<>
		Vector2 ReadFromXML(ResourceSystem::XMLNodeIterator& input);

		template<>
		EntitySystem::EntityHandle ReadFromXML(ResourceSystem::XMLNodeIterator& input);
	}
}

#endif