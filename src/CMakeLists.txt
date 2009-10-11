project(ocerus)

cmake_minimum_required(VERSION 2.6.2)

set(CMAKE_BUILD_TYPE release)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/EntitySystem/EntityMgr
	${CMAKE_CURRENT_SOURCE_DIR}/EntitySystem/ComponentMgr
	${CMAKE_CURRENT_SOURCE_DIR}/Precompiled
	${CMAKE_CURRENT_SOURCE_DIR}/Setup
	${CMAKE_CURRENT_SOURCE_DIR}/Utils
)

## box2d includes and linking

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/../externalLibs/Box2D/Include
	${CMAKE_CURRENT_SOURCE_DIR}/../externalLibs/Box2D/Source
)
	
## cegui
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/../externalLibs/cegui/include
)

## rudeconfig
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/../externalLibs/rudeconfig/include
)


set(Core_SRCS
	Core/Application.cpp
	Core/Config.cpp
	Core/Game.cpp
	Core/LoadingScreen.cpp
	Core/main.cpp
)

set(EntitySystem_SRCS
    EntitySystem/EntityMgr/EntityPicker.cpp
    EntitySystem/EntityMgr/EntityEnums.cpp
    EntitySystem/EntityMgr/EntityDescription.cpp
    EntitySystem/EntityMgr/EntityMgr.cpp
    EntitySystem/EntityMgr/EntityHandle.cpp
    EntitySystem/Components/CmpMaterial.cpp
    EntitySystem/Components/CmpPlatformLogic.cpp
    EntitySystem/Components/CmpPlatformVisual.cpp
    EntitySystem/Components/CmpPlatformPhysics.cpp
    EntitySystem/Components/CmpPlatformParams.cpp
    EntitySystem/ComponentMgr/Component.cpp
    EntitySystem/ComponentMgr/ComponentEnums.cpp
    EntitySystem/ComponentMgr/ComponentMgr.cpp
)

add_executable(ocerus ${Core_SRCS} ${EntitySystem_SRCS})

