# - Try to find CEGUI
# Once done, this will define
#
#  CEGUI_FOUND - system has CEGUI
#  CEGUI_INCLUDE_DIRS - the CEGUI include directories
#  CEGUI_LIBRARIES - link these to use CEGUI

include(FindPkgMacros)
include(FindPkgConfig)

findpkg_begin(CEGUI)

pkg_check_modules(CEGUI CEGUI-OPENGL-0.7.1)
pkg_check_modules(CEGUI REQUIRED CEGUI-OPENGL)

MARK_AS_ADVANCED(
    CEGUI_FOUND
    CEGUI_INCLUDE_DIRS
    CEGUI_LIBRARIES
    CEGUI_LIBRARY_DIRS
)

set(CEGUI_LIBRARIES ${CEGUI_STATIC_LIBRARIES}
	CEGUILibxmlParser
	CEGUIDevILImageCodec
	CEGUIFalagardWRBase)

# message("CEGUI_LIBRARIES: " ${CEGUI_LIBRARIES})

findpkg_finish(CEGUI)
