project(ocerus)

# where to look first for cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

cmake_minimum_required(VERSION 2.6.2)

set(CMAKE_BUILD_TYPE release)

###############################################################################
###                      bundled external libraries                         ###
###############################################################################

###############
# Box2D
###############
ADD_SUBDIRECTORY(externalLibs/Box2D)
set(Box2D_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/Box2D/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/Box2D/Source
)
set(Box2D_LIBRARIES box2d)


###############
# rudeconfig
###############
ADD_SUBDIRECTORY(externalLibs/rudeconfig)
set(rudeconfig_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/rudeconfig/include
)
set(rudeconfig_LIBRARIES rudeconfig)


###############################################################################
###                             dependencies                                ###
###############################################################################


###############
# OIS
###############
find_package(OIS REQUIRED)
if (OIS_FOUND)
    include_directories(${OIS_INCLUDE_DIRS})
else (OIS_FOUND)
    message(FATAL_ERROR "OIS library cannot be found in your system.")
endif (OIS_FOUND)

###############
# Boost
###############
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.36 REQUIRED COMPONENTS system filesystem )
if (Boost_FOUND)
    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${BOOST_INCLUDE_DIRS})
else (Boost_FOUND)
    message(FATAL_ERROR "BOOST library cannot be found in your system.")
endif (Boost_FOUND)

###############
# EXPAT
###############
find_package(EXPAT 2.0.1 REQUIRED)
if (EXPAT_FOUND)
    include_directories(${EXPAT_INCLUDE_DIRS})
else (EXPAT_FOUND)
    message(FATAL_ERROR "Expat library cannot be found in your system.")
    #TODO: use bundled expat
endif (EXPAT_FOUND)

###############
# CEGUI
###############
find_package(CEGUI 0.6.2 REQUIRED)
if (CEGUI_FOUND)
    include_directories(${CEGUI_INCLUDE_DIRS}/CEGUI)
else (CEGUI_FOUND)
    message(FATAL_ERROR "CEGUI library cannot be found in your system.")
    #TODO: use bundled CEGUI
endif (CEGUI_FOUND)



###############################################################################
###                            ocerus project                               ###
###############################################################################

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
    src/Utils/Properties/PropertyTypes.cpp
    src/Utils/RTTI/RTTI.cpp
    src/Utils/Hash.cpp
    src/Utils/GlobalProperties.cpp
    src/Utils/StringConverter.cpp
    src/Utils/SmartAssert.cpp
    src/Utils/Timer.cpp
    src/Utils/MathUtils.cpp
)

include_directories(
    ${Box2D_INCLUDE_DIRS}
    ${rudeconfig_INCLUDE_DIRS}
)

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
    ${Box2D_LIBRARIES}
    ${rudeconfig_LIBRARIES}
    ${OIS_LIBRARIES} 
    ${Boost_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${CEGUI_LIBRARIES}
)



