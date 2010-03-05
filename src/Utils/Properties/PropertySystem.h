/// @file
/// Storage of all properties in the program.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _PROPERTY_SYSTEM_H
#define _PROPERTY_SYSTEM_H

#include "Base.h"
#include "Singleton.h"

namespace Reflection
{
	/// Global manager of all properties.
	/// This class is used to release properties when the program exits.
	namespace PropertySystem
	{
		/// The destructor responsible for releasing the properties.
		void DestroyProperties(void);

		/// Returns the static list of all %RTTI properties in the program.
		/// The properties are released in the destructor.
		std::list<AbstractProperty*>* GetProperties(void);
	}
}

#endif	// _PROPERTY_SYSTEM_H
