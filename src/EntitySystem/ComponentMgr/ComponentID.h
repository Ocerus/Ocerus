/// @file
/// Identifier of components of a single entity. The ID is not globally unique, only with respect to a single entity!

#ifndef ComponentID_h__
#define ComponentID_h__

#include "Base.h"

namespace EntitySystem
{
	/// Identifier of components of a single entity. The ID is not globally unique, only with respect to a single entity!
	typedef int32 ComponentID;
}


#endif // ComponentID_h__