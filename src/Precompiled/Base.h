/// @file
/// Basic file to be included in all header files of the project.
/// @remarks
/// When adding new includes into this file be extremly careful with include dependencies. That to say, don't include
/// files which include a lot of other files. The point of this file is to include only the core files and nothing more.

#ifndef Base_h__
#define Base_h__

// Basic settings and typedefs of the project.
#include "../Setup/Settings.h"

// Forward declarations to minimize usage of #include in headers.
#include "Forwards.h"

// Small headers without dependencies, which are used in a lot of other headers, so that it makes sense to include them here.
#include "../Utils/ResourcePointers.h"


#endif // Base_h__