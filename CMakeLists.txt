project(ocerus)

cmake_minimum_required(VERSION 2.6.2)

set(CMAKE_BUILD_TYPE release)

# search for Boost version 1.36
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.36 COMPONENTS system filesystem )

ADD_SUBDIRECTORY(externalLibs/ois)
ADD_SUBDIRECTORY(externalLibs/rudeconfig)

## box2d includes and linking

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/Box2D/Include
	${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/Box2D/Source
)

## boost includes
#INCLUDE_DIRECTORIES(
#	${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/boost/inc
#)


## cegui
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/cegui/include
)

## rudeconfig
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/rudeconfig/include
)

## ois
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/ois/includes
)



INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/EntitySystem/EntityMgr
	${CMAKE_CURRENT_SOURCE_DIR}/src/EntitySystem/ComponentMgr
	${CMAKE_CURRENT_SOURCE_DIR}/src/Precompiled
	${CMAKE_CURRENT_SOURCE_DIR}/src/Setup
	${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
)

set(Core_SRCS
	src/Core/Application.cpp
	src/Core/Config.cpp
	src/Core/Game.cpp
	src/Core/LoadingScreen.cpp
	src/Core/main.cpp
)

set(EntitySystem_SRCS
    src/EntitySystem/EntityMgr/EntityPicker.cpp
    src/EntitySystem/EntityMgr/EntityEnums.cpp
    src/EntitySystem/EntityMgr/EntityDescription.cpp
    src/EntitySystem/EntityMgr/EntityMgr.cpp
    src/EntitySystem/EntityMgr/EntityHandle.cpp
    src/EntitySystem/Components/CmpMaterial.cpp
    src/EntitySystem/Components/CmpPlatformLogic.cpp
    src/EntitySystem/Components/CmpPlatformVisual.cpp
    src/EntitySystem/Components/CmpPlatformPhysics.cpp
    src/EntitySystem/Components/CmpPlatformParams.cpp
    src/EntitySystem/ComponentMgr/Component.cpp
    src/EntitySystem/ComponentMgr/ComponentEnums.cpp
    src/EntitySystem/ComponentMgr/ComponentMgr.cpp
)

set(GfxSystem_SRCS
    src/GfxSystem/_UnixVoidImpl.cpp
)

set(GUISystem_SRCS
    src/GUISystem/CEGUIResource.cpp
    src/GUISystem/StaticElements.cpp
    src/GUISystem/GUIMgr.cpp
    src/GUISystem/ResourceGate.cpp
    src/GUISystem/CEGUITextureWrapper.cpp
    src/GUISystem/RendererGate.cpp
)

set(InputSystem_SRCS
    src/InputSystem/InputMgr.cpp
    src/InputSystem/OISListener.cpp
)
set(LogSystem_SRCS
    src/LogSystem/LogMgr.cpp
)

set(Memory_SRCS
    src/Memory/GlobalAlloc.cpp
)

set(ResourceSystem_SRCS
    src/ResourceSystem/ResourceMgr.cpp
    src/ResourceSystem/XMLResource.cpp
    src/ResourceSystem/Resource.cpp
)

set(Setup_SRCS
    src/Setup/ComplexTypes.cpp
)

set(StringSystem_SRCS
    src/StringSystem/TextResource.cpp
    src/StringSystem/StringMgr.cpp
)

set(Utils_SRCS
    src/Utils/StringKey.cpp
    src/Utils/Properties/PropertySystem.cpp
    src/Utils/Properties/AbstractProperty.cpp
    src/Utils/Properties/PropertyFunctionParameters.cpp
    src/Utils/Properties/PropertyHolder.cpp
    src/Utils/RTTI/RTTI.cpp
    src/Utils/Hash.cpp
    src/Utils/GlobalProperties.cpp
    src/Utils/StringConverter.cpp
    src/Utils/SmartAssert.cpp
    src/Utils/Timer.cpp
    src/Utils/MathUtils.cpp
)



link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

add_executable(ocerus 
    ${Core_SRCS}
    ${EntitySystem_SRCS}
    ${GfxSystem_SRCS}
    ${GUISystem_SRCS}
    ${InputSystem_SRCS}
    ${LogSystem_SRCS}
    ${Memory_SRCS}
    ${ResourceSystem_SRCS}
    ${Setup_SRCS}
    ${StringSystem_SRCS}
    ${Utils_SRCS}
)

target_link_libraries(ocerus
    ois
    rudeconfig
    ${Boost_LIBRARIES}
)

