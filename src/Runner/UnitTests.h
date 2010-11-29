/// @file
/// Unit tests includes and definitions.

#ifndef UnitTests_h__
#define UnitTests_h__


#include "UnitTest++.h"


/// Helper methods and definitions for unit testing.
namespace Test
{
	/// Inits the basics.
	void Init(bool developMode);

	/// Inits the resource manager.
	void InitResources(void);

	/// Inits the entity manager with no entities.
	void InitEntities(void);

	/// Inits the entity manager and loads entities from the specified file.
	void InitEntities(const string& resourceFileName);

	/// Cleans everything so that the subsystems can be inited again.
	void CleanSubsystems(void);
}


#endif // UnitTests_h__