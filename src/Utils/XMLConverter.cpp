#include "Common.h"
#include "XMLConverter.h"
#include "../ResourceSystem/XMLOutput.h"
#include "../ResourceSystem/XMLResource.h"
#include <cstring>

using namespace XMLConverter;

template<>
void XMLConverter::WriteToXML(ResourceSystem::XMLOutput& output, const Vector2& val)
{
	std::ostringstream out;
	out << val.x << " " << val.y;
	output.WriteString(out.str());
}

template<>
void XMLConverter::WriteToXML(ResourceSystem::XMLOutput& output, const EntitySystem::EntityHandle& val)
{
	output.WriteString(StringConverter::ToString(val.GetID()));
}

template<>
Vector2 XMLConverter::ReadFromXML(ResourceSystem::XMLNodeIterator& input)
{
	Vector2 result;
	std::istringstream iss(input.GetChildValue<string>());
	if ((iss >> result.x).fail()) { return Vector2_Zero; }
	if ((iss >> result.y).fail()) { return Vector2_Zero; }
	return result;
}

template<>
EntitySystem::EntityHandle XMLConverter::ReadFromXML(ResourceSystem::XMLNodeIterator& input)
{
	EntitySystem::EntityID id = input.GetChildValue<EntitySystem::EntityID>();
	return EntitySystem::EntityHandle(id);
}
