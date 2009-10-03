/// @file
/// Lowest level classes for custom classes using the RTTI.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _BASEOBJECT_H_
#define _BASEOBJECT_H_

#include "RTTIGlue.h"

namespace Reflection
{
	/// @brief This is an empty class with no attached %RTTI information.
	/// @remarks We use this class as an ancestor
	///	to our RTTIBaseClass which is the true ultimate ancestor for all our classes. Without the null class
	///	we would have no template parameter for RTTIBaseClass's ancestor. RTTINullClass itself has no RTTI support.
	class RTTINullClass
	{
	protected :

		/// @remarks It is necessary to implement this to avoid compilation errors in the templatized RTTI code
		///	In any case no RTTI support is provided by this class.
		static inline RTTI* GetClassRTTI(void) { return NULL; }

	};

	/// @brief Ultimate ancestor for all classes.
	/// @remarks RTTIBaseClass implements no specific functionality, apart from
	///	the fact it provides the ultimate ancestor RTTI for all other classes.
	class RTTIBaseClass : public RTTIGlue<RTTIBaseClass, RTTINullClass>
	{

	};
}

#endif	// _BASEOBJECT_H_