#include "Common.h"
#include "StringConverter.h"

using namespace StringConverter;

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
bool StringConverter::FromString(const string& str)
{
	bool result;
	std::istringstream iss(str);
	if ((iss >> std::boolalpha >> result).fail())
		return false;
	return result;
}
