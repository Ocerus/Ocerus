#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#include "Base.h"
#include "Singleton.h"
#include "../GfxSystem/GfxStructures.h"

/// Macro for easier use.
#define gSceneMgr SceneSystem::SceneMgr::GetSingleton()

/// Scene system takes care of the logical representation of the scene. This makes the scene editing much easier as the
/// designer is able to group entities together or structure them hierarchicaly.
namespace SceneSystem
{
}

#endif