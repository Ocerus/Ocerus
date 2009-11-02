#include "Common.h"
#include "StringConverter.h"

using namespace StringConverter;

template<>
string StringConverter::ToString(bool& val)
{
	std::ostringstream out;
	out << std::boolalpha << val;
	return out.str();
}
		
template<>
string StringConverter::ToString(bool val)
{
	std::ostringstream out;
	out << std::boolalpha << val;
	return out.str();
}

template<>
char* StringConverter::FromString(const string& str)
{
	char* result = new char[str.length()+1];
	const char* temp = str.c_str();
	strcpy(result, temp);
	return result;
}

template<>
Vector2 StringConverter::FromString(const string& str)
{
	Vector2 result;
	std::istringstream iss(str);
	if ((iss >> result.x).fail())
		return Vector2_Zero;
	if ((iss >> result.y).fail())
		return Vector2_Zero;
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
