IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
RelWithDebInfo MinSizeRel.")
ENDIF()

project(ocerus)

# where to look first for cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

cmake_minimum_required(VERSION 2.6.2)

include(UseDoxygen)
set(DOXYFILE_IN "${CMAKE_SOURCE_DIR}/cmake/modules/Doxyfile.in")

option(PRECOMPILED_HEADERS "Use precompiled headers." OFF)
option(BUILD_TESTS "Builds tests." OFF)
option(USE_DBGLIB "Uses DbgLib for debugging." OFF)
option(USE_LEAKDETECTOR "Uses DbgLib's leak detector." OFF)
option(USE_RTHPROFILER "Uses the included RTHProfiler." OFF)
option(USE_GPROF "Compiles the project for using gprof." OFF)
option(DEPLOY "Compiles the project in DEPLOY mode." OFF)

set(DEPLOY_TARGET_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "Name of the target platform for DEPLOY build.")

set(OCERUS_SHARED_DIR "share/ocerus" CACHE STRING "Path to the ocerus shared data directory (relative to CMAKE_INSTALL_PREFIX)")

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
# SOIL
###############
ADD_SUBDIRECTORY(externalLibs/SOIL)

set(SOIL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/SOIL/src
)
set(SOIL_LIBRARIES SOIL)

list(APPEND ocerus-engine_INCLUDE_DIRS ${SOIL_INCLUDE_DIRS})
list(APPEND ocerus-engine_LIBRARIES ${SOIL_LIBRARIES})

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

if (USE_RTHPROFILER)
	ADD_SUBDIRECTORY(externalLibs/RTHProfiler)
	set(RTHProfiler_INCLUDE_DIRS
		${CMAKE_CURRENT_SOURCE_DIR}/externalLibs/RTHProfiler/include
	)
	set(RTHProfiler_LIBRARIES RTHProfiler)

	list(APPEND ocerus-engine_INCLUDE_DIRS ${RTHProfiler_INCLUDE_DIRS})
	list(APPEND ocerus-engine_LIBRARIES ${RTHProfiler_LIBRARIES})

	list(APPEND ocerus-engine_SYMBOLS USE_PROFILER)
endif (USE_RTHPROFILER)

if (USE_GPROF)
	message("USING GPROF")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif (USE_GPROF)

if (DEPLOY)
	message("Compiling DEPLOY version.")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEPLOY")
endif (DEPLOY)


###############################################################################
###                             dependencies                                ###
###############################################################################

###############
# CEGUI
###############
find_package(CEGUI 0.7.1 REQUIRED)
if (CEGUI_FOUND)
    include_directories(BEFORE SYSTEM ${CEGUI_INCLUDE_DIRS})
else (CEGUI_FOUND)
    message(FATAL_ERROR "CEGUI library cannot be found in your system.")
endif (CEGUI_FOUND)


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
find_package( Boost 1.36 REQUIRED COMPONENTS system filesystem regex )
if (Boost_FOUND)
    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
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
# UnitTest++
###############
if (BUILD_TESTS)
	find_package(UnitTest++ 1.4 REQUIRED)
	if (UNITTEST++_FOUND)
		include_directories(${UNITTEST++_INCLUDE_DIR})
	else (UNITTEST++_FOUND)
		message(FATAL_ERROR "UnitTest++ library cannot be found in your system.")
	endif (UNITTEST++_FOUND)
endif (BUILD_TESTS)

###############
# ICU
###############
#find_package(ICU 4.2 REQUIRED)
#if (ICU_FOUND)
#    include_directories(${ICU_INCLUDE_DIRS})
#else (ICU_FOUND)
#    message(FATAL_ERROR "ICU library cannot be found in your system.")
#endif (ICU_FOUND)


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
	src/Core/Project.cpp
	src/Core/LoadingScreen.cpp
)

set(Editor_SRCS
	src/Editor/CreateProjectDialog.cpp
	src/Editor/EditorGUI.cpp
	src/Editor/EditorMenu.cpp
	src/Editor/EditorMgr.cpp
	src/Editor/EntityWindow.cpp
	src/Editor/HierarchyWindow.cpp
	src/Editor/KeyShortcuts.cpp
	src/Editor/LayerWindow.cpp
	src/Editor/PrototypeWindow.cpp
	src/Editor/ResourceWindow.cpp
	src/Editor/ValueEditors/AbstractValueEditor.cpp
	src/Editor/ValueEditors/ArrayEditor.cpp
	src/Editor/ValueEditors/StringEditor.cpp
	src/Editor/ValueEditors/PrototypeEditor.cpp
	src/Editor/ValueEditors/ResourceEditor.cpp
	src/Editor/ValueEditors/BoolEditor.cpp
	src/Editor/ValueEditors/TwoDimEditor.cpp
	src/Editor/ValueEditors/ValueEditorFactory.cpp
	src/Editor/ValueEditors/Models/EntityAttributeModel.cpp
	src/Editor/ValueEditors/Models/StringValueEditorModel.cpp
)

set(EntitySystem_SRCS
	src/EntitySystem/EntityMgr/EntityPicker.cpp
	src/EntitySystem/EntityMgr/EntityDescription.cpp
	src/EntitySystem/EntityMgr/EntityMgr.cpp
	src/EntitySystem/EntityMgr/EntityHandle.cpp
	src/EntitySystem/EntityMgr/EntityMessage.cpp
	src/EntitySystem/EntityMgr/LayerMgr.cpp
	src/EntitySystem/Components/Camera.cpp
	src/EntitySystem/Components/DynamicBody.cpp
	src/EntitySystem/Components/GUILayout.cpp
	src/EntitySystem/Components/Model.cpp
	src/EntitySystem/Components/PolygonCollider.cpp
	src/EntitySystem/Components/Script.cpp
	src/EntitySystem/Components/Sprite.cpp
	src/EntitySystem/Components/StaticBody.cpp
	src/EntitySystem/Components/Transform.cpp
	src/EntitySystem/ComponentMgr/Component.cpp
	src/EntitySystem/ComponentMgr/ComponentEnums.cpp
	src/EntitySystem/ComponentMgr/ComponentMgr.cpp
)

set(GfxSystem_SRCS
	src/GfxSystem/DragDropCameraMover.cpp
	src/GfxSystem/GfxRenderer.cpp
	src/GfxSystem/GfxSceneMgr.cpp
	src/GfxSystem/GfxStructures.cpp
	src/GfxSystem/GfxViewport.cpp
	src/GfxSystem/GfxWindow.cpp
	src/GfxSystem/PhysicsDraw.cpp
	src/GfxSystem/Mesh.cpp
	src/GfxSystem/OglRenderer.cpp
	src/GfxSystem/Texture.cpp
	src/GfxSystem/objloader/model_obj.cpp
)

set(SceneSystem_SRCS

)

set(GUISystem_SRCS
	src/GUISystem/ResourceProvider.cpp
	src/GUISystem/CEGUIResource.cpp
	src/GUISystem/GUIMgr.cpp
	src/GUISystem/GUIConsole.cpp
	src/GUISystem/MessageBox.cpp
	src/GUISystem/PopupMgr.cpp
	src/GUISystem/PromptBox.cpp
	src/GUISystem/VerticalLayout.cpp
	src/GUISystem/ViewportWindow.cpp
	src/GUISystem/FolderSelector.cpp
	src/GUISystem/ScriptProvider.cpp
	src/GUISystem/TabNavigation.cpp
)

set(InputSystem_SRCS
	src/InputSystem/InputActions.cpp
	src/InputSystem/InputMgr.cpp
	src/InputSystem/OISListener.cpp
)

set(LogSystem_SRCS
	src/LogSystem/Logger.cpp
	src/LogSystem/LogMgr.cpp
	src/LogSystem/Profiler.cpp
)

set(Memory_SRCS
	src/Memory/GlobalAllocation.cpp
	src/Memory/FreeListPolicyHelpers.cpp
)

set(ResourceSystem_SRCS
	src/ResourceSystem/ResourceMgr.cpp
	src/ResourceSystem/XMLResource.cpp
	src/ResourceSystem/Resource.cpp
	src/ResourceSystem/ResourceTypes.cpp
	src/ResourceSystem/XMLOutput.cpp
)

set(ScriptSystem_SRCS
	src/ScriptSystem/ScriptMgr.cpp
	src/ScriptSystem/ScriptRegister.cpp
	src/ScriptSystem/ScriptResource.cpp
	src/ScriptSystem/AddOn/scriptstring.cpp
	src/ScriptSystem/AddOn/scriptbuilder.cpp
)

set(Setup_SRCS
	src/Setup/ComplexTypes.cpp
)

set(StringSystem_SRCS
    src/StringSystem/FormatText.cpp
	src/StringSystem/TextResource.cpp
	src/StringSystem/StringMgr.cpp
)

set(Utils_SRCS
	src/Utils/FilesystemUtils.cpp
	src/Utils/StringKey.cpp
	src/Utils/Properties/PropertySystem.cpp
	src/Utils/Properties/AbstractProperty.cpp
	src/Utils/Properties/PropertyFunctionParameters.cpp
	src/Utils/Properties/PropertyHolder.cpp
	src/Utils/Properties/PropertyEnums.cpp
	src/Utils/Properties/PropertyMap.cpp
	src/Utils/RTTI/RTTI.cpp
	src/Utils/Hash.cpp
	src/Utils/GlobalProperties.cpp
	src/Utils/StringConverter.cpp
	src/Utils/SmartAssert.cpp
	src/Utils/Timer.cpp
	src/Utils/MathUtils.cpp
	src/Utils/XMLConverter.cpp
)

include_directories(${ocerus-engine_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x ")

foreach(SYMBOL ${ocerus-engine_SYMBOLS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${SYMBOL}")
endforeach(SYMBOL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOCERUS_SHARED_DIR=\"\\\"${CMAKE_INSTALL_PREFIX}/${OCERUS_SHARED_DIR}\\\"\" ")

set(CMAKE_CXX_FLAGS_RELEASE "-O2 ")
#set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wextra -pedantic -Wno-variadic-macros -Wno-long-long -DUSE_ASSERT -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wextra -pedantic -DUSE_ASSERT -D_DEBUG")

add_library(ocerus-engine STATIC
    ${Core_SRCS}
    ${Editor_SRCS}
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

if (PRECOMPILED_HEADERS)
    include("PCHSupport_26")
    add_precompiled_header(ocerus-engine ${CMAKE_CURRENT_SOURCE_DIR}/src/Precompiled/Common.h)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpch-preprocess -fpch-deps -Winvalid-pch")
endif (PRECOMPILED_HEADERS)


link_directories(
    ${CEGUI_LIBRARY_DIRS}
)

target_link_libraries(ocerus-engine
    ${ocerus-engine_LIBRARIES}
    ${OIS_LIBRARIES}
    ${Boost_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${SDL_LIBRARY}
    ${CEGUI_LIBRARIES}
    ${ICU_LIBRARIES}
)

if (BUILD_TESTS)
	target_link_libraries(ocerus-engine ${UNITTEST++_LIBRARY})
endif (BUILD_TESTS)

# Create symlinks
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/output/data ${CMAKE_BINARY_DIR}/data)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/output/deploy ${CMAKE_BINARY_DIR}/deploy)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/output/projects ${CMAKE_BINARY_DIR}/projects)

message("Ocerus build configuration")
message("Build type: ${CMAKE_BUILD_TYPE}")
message("Install prefix: ${CMAKE_INSTALL_PREFIX}")
message("Ocerus shared dir: ${OCERUS_SHARED_DIR}")

message("CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})


add_executable(ocerus-main src/Runner/main.cpp)


target_link_libraries(ocerus-main ocerus-engine)

###############################################################################
###                             Ocerus tests                                ###
###############################################################################
if (BUILD_TESTS)
    file(GLOB Tests_SRCS src/*/test/Test*.cpp src/*/*/test/Test*.cpp src/*/*/*/test/Test*.cpp)

#    foreach (test ${Tests_SRCS})
#    	message("Adding test: ${test}")
#    endforeach()

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Runner
    )

    add_executable(ocerus-test src/Runner/UnitTests.cpp ${Tests_SRCS})

    set_target_properties(ocerus-test PROPERTIES COMPILE_FLAGS "-DUNIT_TESTS")

    target_link_libraries(ocerus-test ocerus-engine)

	execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/output/test ${CMAKE_BINARY_DIR}/test)
endif (BUILD_TESTS)



###############################################################################
###                             Installation                                ###
###############################################################################

if (DEPLOY)
	install(TARGETS ocerus-main RUNTIME DESTINATION "${CMAKE_SOURCE_DIR}/output/deploy/${DEPLOY_TARGET_NAME}")
else (DEPLOY)
	set_property(TARGET ocerus-main PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
	install(TARGETS ocerus-main RUNTIME DESTINATION bin)
	install(DIRECTORY output/deploy output/data output/projects
			DESTINATION ${OCERUS_SHARED_DIR}
			PATTERN ".svn" EXCLUDE)
endif (DEPLOY)
