/// @file
/// Mediator for transfer of PropertyHolder between methods without the need of knowing PropertyHolder.
/// This step is needed to break the circular dependencies.

#ifndef PropertyHolderMediator_h__
#define PropertyHolderMediator_h__

#include "Base.h"

namespace Reflection
{
	/// @brief This is a mediator for the transfer of PropertyHolder between methods without the need of actually knowing PropertyHolder.
	/// This step is needed to break circular dependencies.
	/// @remarks The mediator itself must have the same size as PropertyHolder so that the pointer conversions between
	/// PropertyHolder and PropertyHolderMediator are valid. That's why uint64 is used.
	typedef uint64 PropertyHolderMediator;
}

#endif // PropertyHolderMediator_h__