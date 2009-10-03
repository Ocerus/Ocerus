/// @file
/// Basic file to be included in all header files of the project.
/// @remarks
/// When adding new includes into this file be extremly careful with include dependencies. That to say, don't include
/// files which include a lot of other files. The point of this file is to include only the core files and nothing more.

#ifndef Base_h__
#define Base_h__

#include "../Setup/Settings.h"
#include "Forwards.h"

#include "../Utils/ResourcePointers.h"
#include "../Utils/Properties/PropertyHolderMediator.h"


#endif // Base_h__