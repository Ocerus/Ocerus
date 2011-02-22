#include "Common.h"
#include "ComplexTypes.h"

const Vector2 Vector2_Zero(0.0f, 0.0f);
Vector2 Vector2_Dummy(0.0f, 0.0f);
const Matrix22 Matrix22_Identity(1.0f,0.0f,0.0f,1.0f);
const XForm XForm_Identity(Vector2_Zero, Matrix22_Identity);

bool IsStringValid( const std::string& str )
{
	for (size_t i=0; i<str.size(); ++i)
	{
		if (!((unsigned)(str[i] + 1) <= 256)) return false;
	}
	return true;
}