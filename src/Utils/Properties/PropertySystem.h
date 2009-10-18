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
	class PropertySystem : protected Singleton<PropertySystem>
	{
	public :

		/// The destructor responsible for releasing the properties.
		~PropertySystem();

		/// Returns the static list of all %RTTI properties in the program.
		/// The proeprties are released in the destructor.
		static std::list<AbstractProperty*>* GetProperties(void);
	};
}

#endif	// _PROPERTY_SYSTEM_H
