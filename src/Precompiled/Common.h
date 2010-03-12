/// @file
/// Precompiled headers to be included in ALL source files. They must be included as the first header!

#ifndef _COMMON_H_
#define _COMMON_H_


#include "../Setup/Settings.h"


// Big standard libraries come here. But only if they are needed by some of the headers below.
#include <boost/filesystem/fstream.hpp>

// Our custom headers come here.
#include "../Core/Config.h"

// We may need to use Logger before other headers
#include "../LogSystem/Logger.h"
#include "../LogSystem/Profiler.h"
#include "../LogSystem/LogMacros.h"

#include "../Utils/Array.h"
#include "../Utils/DataContainer.h"
#include "../Utils/Hash.h"
#include "../Utils/MathUtils.h"
#include "../Utils/Properties.h"
#include "../Utils/ResourcePointers.h"
#include "../Utils/SmartAssert.h"
#include "../Utils/StringConverter.h"
#include "../Utils/StringKey.h"
#include "../Utils/Timer.h"
#include "../Utils/GlobalProperties.h"

#include "../EntitySystem/EntityMgr/EntityMgr.h"
#include "../EntitySystem/EntityMgr/EntityHandle.h"
#include "../EntitySystem/EntityMgr/EntityHandleInlines.h"
#include "../EntitySystem/EntityMgr/EntityDescription.h"
#include "../EntitySystem/ComponentMgr/ComponentEnums.h"
#include "../EntitySystem/EntityMgr/EntityPicker.h"

#include "../GfxSystem/GfxWindow.h"
#include "../GfxSystem/GfxRenderer.h"

#include "../GUISystem/GUIMgr.h"

#include "../InputSystem/InputMgr.h"

#include "../ResourceSystem/ResourceMgr.h"
#include "../ResourceSystem/Resource.h"

#include "../ScriptSystem/ScriptMgr.h"

#include "../StringSystem/StringMgr.h"



#endif
