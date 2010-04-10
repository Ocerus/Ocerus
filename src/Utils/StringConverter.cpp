#include "Common.h"
#include "StringConverter.h"
#include <cstring>

using namespace StringConverter;

template<>
string StringConverter::ToString(const Vector2& val)
{
	std::ostringstream out;
	out << "(" << val.x << ", " <<  val.y << ")";
	return out.str();
}

template<>
string StringConverter::ToString(const GfxSystem::Color& val)
{
	std::ostringstream out;
	out << std::hex << val.r << val.g << val.b << val.a;
	return out.str();
}

template<>
string StringConverter::ToString(const ResourceSystem::ResourcePtr& val)
{
	if (!val) return "NULL";

	return val->GetName();
}

template<>
string StringConverter::ToString(const bool& val)
{
	std::ostringstream out;
	out << std::boolalpha << val;
	return out.str();
}

template<>
string StringConverter::ToString(const PropertyFunctionParameters& val)
{
	OC_UNUSED(val);
	ocError << "Converting value of type 'PropertyFunctionParameters' to string is not implemented.";
	return "";
}

template<>
string StringConverter::ToString(const EntitySystem::EntityHandle& val)
{
	if (EntitySystem::EntityMgr::SingletonExists())
	{
		return EntitySystem::EntityMgr::GetSingleton().GetEntityName(val);
	}
	else
	{
		return StringConverter::ToString(val.GetID());
	}
}

template<>
Vector2 StringConverter::FromString(const string& str)
{
	Vector2 result;
	char c;
	std::istringstream iss(str);
	if (iss.peek() == '(')
		iss >> c;

	if ((iss >> result.x).fail())
		return Vector2_Zero;

	if (iss.peek() == ',')
		iss >> c;

	if ((iss >> result.y).fail())
		return Vector2_Zero;

	if (iss.peek() == ')')
		iss >> c;
	return result;
}

template<>
GfxSystem::Color StringConverter::FromString(const string& str)
{
	GfxSystem::Color result;
	uint32 val;
	std::istringstream iss(str);
	if ((iss >> val).fail())
		return GfxSystem::Color::Null;
	result.r = (uint8)val;
	if ((iss >> val).fail())
		return GfxSystem::Color::Null;
	result.g = (uint8)val;
	if ((iss >> val).fail())
		return GfxSystem::Color::Null;
	result.b = (uint8)val;
	if (!(iss >> val).fail())
		result.a = (uint8)val;
	return result;
}

template<>
PropertyFunctionParameters StringConverter::FromString(const string& str)
{
	ocError << "Converting string '" << str << "' to PropertyFunctionParameters is not valid conversion.";
	return PropertyFunctionParameters::Null;
}

template<>
EntitySystem::EntityHandle StringConverter::FromString(const string& str)
{
	if (!EntitySystem::EntityMgr::SingletonExists())
	{
		ocError << "Cannot convert string '" << str << "' to EntityHandle, because EntityMgr does not exist.";
		return EntitySystem::EntityHandle::Null;
	}
	
	EntitySystem::EntityHandle result = gEntityMgr.FindFirstEntity(str);
	if (!result.IsValid())
	{
		ocError << "Cannot convert string '" << str << "' to EntityHandle, because no such entity exists.";
		return EntitySystem::EntityHandle::Null;
	}
	return result;
}


template<>
bool StringConverter::FromString(const string& str)
{
	bool result;
	std::istringstream iss(str);
	if ((iss >> std::boolalpha >> result).fail())
		return false;
	return result;
}

template<>
StringKey StringConverter::FromString(const string& str)
{
	return StringKey(str);
}

template<>
ResourceSystem::ResourcePtr StringConverter::FromString(const string& str)
{
	ocError << "Converting string '" << str << "' to ResourcePtr is not valid conversion.";
	return ResourceSystem::ResourcePtr();
}

template<>
PhysicalShape* StringConverter::FromString(const string& str)
{
	ocError << "Converting string '" << str << "' to PhysicalShape* is not valid conversion.";
	return 0;
}

template<>
PhysicalBody* StringConverter::FromString(const string& str)
{
	ocError << "Converting string '" << str << "' to PhysicalBody* is not valid conversion.";
	return 0;
}
