/// @file
/// Precompiled headers to be included in ALL source files.

#ifndef _COMMON_H_
#define _COMMON_H_


#include "../Setup/Settings.h"


/// Big standard libraries come here. But only if they are needed by some of the headers below.
#include <boost/filesystem/fstream.hpp>


/// Our custom headers come here.
#include "../Core/Config.h"

#include "../EntitySystem/EntityMgr/EntityMgr.h"
#include "../EntitySystem/EntityMgr/EntityHandle.h"
#include "../EntitySystem/EntityMgr/EntityMessage.h"
#include "../EntitySystem/EntityMgr/EntityDescription.h"
#include "../EntitySystem/ComponentMgr/ComponentEnums.h"
#include "../EntitySystem/EntityMgr/EntityEnums.h"
#include "../EntitySystem/EntityMgr/EntityPicker.h"

#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/ParticleSystemMgr.h"

#include "../GUISystem/GUIMgr.h"

#include "../InputSystem/InputMgr.h"

#include "../LogSystem/LogMgr.h"

#include "../ResourceSystem/ResourceMgr.h"
#include "../ResourceSystem/Resource.h"

#include "../StringSystem/StringMgr.h"

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


#endif