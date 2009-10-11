#project()

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


set(PROJECT_SRCS
	Core/Application.cpp
	Core/Config.cpp
	Core/Game.cpp
	Core/LoadingScreen.cpp
	Core/main.cpp)

add_executable(project ${PROJECT_SRCS})
