#builds the DbgLib library

cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

set(SRC_FILES
    DbgLib/Utils.cpp
    DbgLib/DebugFx.cpp
    DbgLib/stdafx.cpp
    DbgLib/DebugHelp.cpp
    DbgLib/MemLeakDetector.cpp
    DbgLib/ThreadLister.c
    DbgLib/LinuxThreads.c
    DbgLib/CoreDumper.c
    DbgLib/ELFCore.c
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/DbgLib
)

add_library( DbgLib ${SRC_FILES} )


