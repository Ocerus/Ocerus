#builds the ois library

cmake_minimum_required(VERSION 2.6)


set(SRC_FILES    "src/OISEffect.cpp"
                 "src/OISForceFeedback.cpp"
                 "src/OISInputManager.cpp"
                 "src/OISJoyStick.cpp"
                 "src/OISKeyboard.cpp"
                 "src/OISObject.cpp")

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/includes")

set(LIBS "")
set(LINK_DIRS "")

if(WIN32)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

    find_package(DirectX)

    set(INCLUDE_DIR ${INCLUDE_DIR}
"${TSLRPG_SOURCE_DIR}/external/ois-1.0RC1/includes/win32"
${DirectX_INCLUDE_DIR})

    set(LIBS ${LIBS} dxguid dinput8)

    set(LINK_DIRS ${LINK_DIRS} ${DirectX_LIB_DIR})

    set(SRC_FILES   ${SRC_FILES}
                    "src/win32/Win32ForceFeedBack.cpp"
                    "src/win32/Win32InputManager.cpp"
                    "src/win32/Win32JoyStick.cpp"
                    "src/win32/Win32KeyBoard.cpp"
                    "src/win32/Win32Mouse.cpp")

endif(WIN32)

include_directories( ${INCLUDE_DIR} )
link_directories( ${LINK_DIRS} )
add_library( ois ${SRC_FILES} )
target_link_libraries( ois ${LIBS})

#Here is the code from FindDirectX.cmake

# - Test for DirectX
# Once loaded this will define
#   DirectX_FOUND        - system has DirectX
#   DirectX_INCLUDE_DIR  - include directory for DirectX
#   DirectX_LIB_DIR      - lib directory for DirectX

set(DirectX_FOUND "NO")

if(WIN32)
    find_path(DirectX_INCLUDE_DIR "dinput.h"
    "C:/Program Files/Microsoft DirectX SDK (October 2006)/Include"
        "C:/Program Files/Microsoft DirectX SDK (February 2007)/Include"
        "C:/Program Files/Microsoft Visual Studio .NET
2003/Vc7/PlatformSDK/Include"
        "C:/Program Files/Microsoft DirectX SDK (February 2006)/Include"
        "C:/Program Files/Microsoft DirectX 9.0 SDK (June 2005)/Include"
        "C:/DXSDK/Include")

    find_path(DirectX_LIB_DIR "dinput8.lib"
        "C:/Program Files/Microsoft DirectX SDK (October 2006)/Lib/x86"
        "C:/Program Files/Microsoft DirectX SDK (February 2007)/Lib/x86"
        "C:/Program Files/Microsoft Visual Studio .NET
2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft DirectX SDK (February 2006)/Lib"
        "C:/Program Files/Microsoft DirectX 9.0 SDK (June 2005)/Lib"
        "C:/DXSDK/Lib")

    if(DirectX_INCLUDE_DIR AND DirectX_LIB_DIR)
        set(DirectX_FOUND "YES")
    endif(DirectX_INCLUDE_DIR AND DirectX_LIB_DIR)
else(WIN32)
endif(WIN32)

if (DirectX_FOUND)
   if (NOT DirectX_FIND_QUIETLY)
      message(STATUS "Found DirectX: ${DirectX_LIBRARIES}")
   endif (NOT DirectX_FIND_QUIETLY)
else (DirectX_FOUND)
   if (DirectX_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find DirectX")
   endif (DirectX_FIND_REQUIRED)
endif (DirectX_FOUND)

