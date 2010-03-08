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
	ocError << "Converting value of type 'PropertyFunctionParameters' to string is not implemented";
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


/*
template<>
string StringConverter::ToString(bool val)
{
	std::ostringstream out;
	out << std::boolalpha << val;
	return out.str();
}
*/

template<>
char* StringConverter::FromString(const string& str)
{
	OC_UNUSED(str);
	OC_ASSERT_MSG(false, "Use .c_str() instead, noob!");
	return 0;
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
		return GfxSystem::Color::NullColor;
	result.r = (uint8)val;
	if ((iss >> val).fail())
		return GfxSystem::Color::NullColor;
	result.g = (uint8)val;
	if ((iss >> val).fail())
		return GfxSystem::Color::NullColor;
	result.b = (uint8)val;
	if (!(iss >> val).fail())
		result.a = (uint8)val;
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
