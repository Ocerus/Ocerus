#include "Common.h"
#include "XMLConverter.h"
#include "../ResourceSystem/XMLOutput.h"
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