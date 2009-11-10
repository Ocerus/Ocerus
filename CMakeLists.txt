project(ocerus)

# where to look first for cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

cmake_minimum_required(VERSION 2.6.2)

option(BUILD_TESTS "Builds tests." ON)
option(USE_DBGLIB "Uses DbgLib for debugging." ON)
option(USE_LEAKDETECTOR "Uses DbgLib's leak detector." ON)

set(ocerus-engine_INCLUDE_DIRS)
set(ocerus-engine_LIBRARIES)
set(ocerus-engine_SYMBOLS)


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
list(APPEND ocerus-engine_INCLUDE_DIRS ${Box2D_INCLUDE_DIRS})
list(APPEND ocerus-engine_LIBRARIES ${Box2D_LIBRARIES})

###############
# rudeconfig
###############
ADD_SUBDIRECTORY(externalLibs/rudeconfig)

set(rudeconfig_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/rudeconfig/include
)
set(rudeconfig_LIBRARIES rudeconfig)

list(APPEND ocerus-engine_INCLUDE_DIRS ${rudeconfig_INCLUDE_DIRS})
list(APPEND ocerus-engine_LIBRARIES ${rudeconfig_LIBRARIES})

###############
# angelscript
###############
ADD_SUBDIRECTORY(externalLibs/angelscript)

set(angelscript_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/angelscript/include
)
set(angelscript_LIBRARIES angelscript)

list(APPEND ocerus-engine_INCLUDE_DIRS ${angelscript_INCLUDE_DIRS})
list(APPEND ocerus-engine_LIBRARIES ${angelscript_LIBRARIES})

###############
# DbgLib
###############
if (USE_DBGLIB OR USE_LEAKDETECTOR)

    ADD_SUBDIRECTORY(externalLibs/DbgLib)

    set(DbgLib_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/DbgLib
    )
    set(DbgLib_LIBRARIES DbgLib)

    list(APPEND ocerus-engine_INCLUDE_DIRS ${DbgLib_INCLUDE_DIRS})
    list(APPEND ocerus-engine_LIBRARIES ${DbgLib_LIBRARIES})

    list(APPEND ocerus-engine_SYMBOLS USE_DBGLIB)
    if (USE_LEAKDETECTOR)
        list(APPEND ocerus-engine_SYMBOLS USE_LEAKDETECTOR)
    endif (USE_LEAKDETECTOR)

endif (USE_DBGLIB OR USE_LEAKDETECTOR)

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
endif (EXPAT_FOUND)

###############
# SDL
###############
#find_package(OpenGL 7.0 REQUIRED)
find_package(SDL 1.2 REQUIRED)
#find_package(SDL_image REQUIRED)
if (SDL_FOUND)
    include_directories(${SDL_INCLUDE_DIR})
else (SDL_FOUND)
    message(FATAL_ERROR "SDL library cannot be found in your system.")
endif (SDL_FOUND)

###############
# CEGUI
###############
find_package(CEGUI 0.6.2 REQUIRED)
if (CEGUI_FOUND)
    include_directories(${CEGUI_INCLUDE_DIRS}/CEGUI)
else (CEGUI_FOUND)
    message(FATAL_ERROR "CEGUI library cannot be found in your system.")
endif (CEGUI_FOUND)

###############
# UnitTest++
###############
find_package(UnitTest++ 1.4 REQUIRED)
if (UNITTEST++_FOUND)
    include_directories(${UNITTEST++_INCLUDE_DIR})
else (UNITTEST++_FOUND)
    message(FATAL_ERROR "UnitTest++ library cannot be found in your system.")
endif (UNITTEST++_FOUND)

###############
# ICU
###############
find_package(ICU 4.2 REQUIRED)
if (ICU_FOUND)
    include_directories(${ICU_INCLUDE_DIRS})
else (ICU_FOUND)
    message(FATAL_ERROR "ICU library cannot be found in your system.")
endif (ICU_FOUND)




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
    src/EntitySystem/Components/Transform.cpp
    src/EntitySystem/Components/OnInitScript.cpp
    src/EntitySystem/ComponentMgr/Component.cpp
    src/EntitySystem/ComponentMgr/ComponentEnums.cpp
    src/EntitySystem/ComponentMgr/ComponentMgr.cpp
)

set(GfxSystem_SRCS
    src/GfxSystem/GfxRenderer.cpp
    src/GfxSystem/GfxStructures.cpp
    src/GfxSystem/OGlRenderer.cpp
    src/GfxSystem/Texture.cpp
)

set(SceneSystem_SRCS
    src/SceneSystem/GfxWindow.cpp
    src/SceneSystem/SceneMgr.cpp
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
    src/LogSystem/Logger.cpp
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

set(ScriptSystem_SRCS
    src/ScriptSystem/AddOn/scriptstring.cpp
    src/ScriptSystem/ScriptMgr.cpp
    src/ScriptSystem/ScriptResource.cpp
    src/ScriptSystem/AddOn/scriptbuilder.cpp
)

set(Setup_SRCS
    src/Setup/ComplexTypes.cpp
)

set(StringSystem_SRCS
    #    src/StringSystem/TextData.cpp
    src/StringSystem/TextResource.cpp
    src/StringSystem/StringMgr.cpp
)

set(Utils_SRCS
    src/Utils/StringKey.cpp
    src/Utils/Properties/PropertySystem.cpp
    src/Utils/Properties/AbstractProperty.cpp
    src/Utils/Properties/PropertyFunctionParameters.cpp
    src/Utils/Properties/PropertyHolder.cpp
    src/Utils/Properties/PropertyEnums.cpp
    src/Utils/RTTI/RTTI.cpp
    src/Utils/Hash.cpp
    src/Utils/GlobalProperties.cpp
    src/Utils/StringConverter.cpp
    src/Utils/SmartAssert.cpp
    src/Utils/Timer.cpp
    src/Utils/MathUtils.cpp
)

include_directories(${ocerus-engine_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -DUSE_ASSERT -D_DEBUG")

foreach(SYMBOL ${ocerus-engine_SYMBOLS})
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D${SYMBOL}")
endforeach(SYMBOL)

add_library(ocerus-engine STATIC
    ${Core_SRCS}
    ${EntitySystem_SRCS}
    ${GfxSystem_SRCS}
    ${SceneSystem_SRCS}
    ${GUISystem_SRCS}
    ${InputSystem_SRCS}
    ${LogSystem_SRCS}
    ${Memory_SRCS}
    ${ResourceSystem_SRCS}
    ${ScriptSystem_SRCS}
    ${Setup_SRCS}
    ${StringSystem_SRCS}
    ${Utils_SRCS}
)

target_link_libraries(ocerus-engine
    ${ocerus-engine_LIBRARIES}
    ${OIS_LIBRARIES}
    ${Boost_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${SDL_LIBRARY}
    ${CEGUI_LIBRARIES}
    ${UNITTEST++_LIBRARY}
    ${ICU_LIBRARIES}
)

add_executable(ocerus-main src/Runner/main.cpp)

target_link_libraries(ocerus-main ocerus-engine)

###############################################################################
###                             Ocerus tests                                ###
###############################################################################
if (BUILD_TESTS)
    file(GLOB Tests_SRCS src/*/test/Test*.cpp src/*/*/test/Test*.cpp src/*/*/*/test/Test*.cpp)

    foreach (test ${Tests_SRCS})
    	message("Adding test: ${test}")
    endforeach()
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Runner
    )

    add_executable(ocerus-test src/Runner/UnitTests.cpp ${Tests_SRCS})

    set_target_properties(ocerus-test PROPERTIES COMPILE_FLAGS "-DUNIT_TESTS")

    target_link_libraries(ocerus-test ocerus-engine)
endif (BUILD_TESTS)
