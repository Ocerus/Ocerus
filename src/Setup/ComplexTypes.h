/// @file
/// Platform specific setting of complex (usually structured) data types.
/// @remarks The current setting tries to shadow STL containers and data structures to provide higher
/// level of encapsulation and to enable easier changes in the future.

#ifndef ComplexTypes_h__
#define ComplexTypes_h__

#include "BasicTypes.h"
#include "../../externalLibs/Box2D/Source/Box2D/Common/b2Math.h"

typedef b2Vec2 Vector2;
extern const Vector2 Vector2_Zero;
extern Vector2 Vector2_Dummy;
typedef b2Mat22 Matrix22;
extern const Matrix22 Matrix22_Identity;
typedef b2Transform XForm;
extern const XForm XForm_Identity;

inline bool operator != (const Vector2& a, const Vector2& b) { return a.x != b.x || a.y != b.y; }

class b2World;
typedef b2World Physics;
class b2Body;
typedef b2Body PhysicalBody;
class b2Fixture;
typedef b2Fixture PhysicalShape;


#include <string>
typedef std::string string;
bool IsStringValid(const std::string& str);


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

#include <sstream>
typedef std::stringstream stringstream;
typedef std::ostringstream ostringstream;


#endif // ComplexTypes_h__
