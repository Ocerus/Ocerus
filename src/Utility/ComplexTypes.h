#ifndef ComplexTypes_h__
#define ComplexTypes_h__

#include "BasicTypes.h"
#include "../Source/Common/b2Math.h"

typedef b2Vec2 Vector2;
extern const Vector2 Vector2_Zero;
extern Vector2 Vector2_Dummy;
typedef b2Mat22 Matrix22;
extern const Matrix22 Matrix22_Identity;
typedef b2XForm XForm;

#include <string>
typedef std::string String;

#include "Containers.h"

#include <ios>
typedef std::istream InputStream;
enum eInputStreamMode { ISM_TEXT, ISM_BINARY };
inline std::ios_base::openmode InputStreamMode(eInputStreamMode mode)
{
	switch (mode)
	{
	case ISM_TEXT:
		return std::ios_base::in;
	case ISM_BINARY:
		return std::ios_base::in | std::ios_base::binary;
	}
	return std::ios_base::in | std::ios_base::binary;
}


#endif // ComplexTypes_h__