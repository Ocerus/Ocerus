# - Try to find CEGUI
# Once done, this will define
#
#  CEGUI_FOUND - system has CEGUI
#  CEGUI_INCLUDE_DIRS - the CEGUI include directories
#  CEGUI_LIBRARIES - link these to use CEGUI

include(FindPkgMacros)
include(FindPkgConfig)

findpkg_begin(CEGUI)

pkg_check_modules(CEGUI REQUIRED CEGUI-0.7.1)
SET(CEGUI_INCLUDE_DIRS ${CEGUI_INCLUDE_DIRS})
SET(CEGUI_LIBRARIES ${CEGUI_LIBRARIES})

MARK_AS_ADVANCED(
    CEGUI_FOUND
    CEGUI_INCLUDE_DIRS
    CEGUI_LIBRARIES
    CEGUI_LIBRARY_DIRS
)

SET(CEGUI_LIBRARIES CEGUIBase-0.7.1 CEGUIOpenGLRenderer-0.7.1 CEGUILibxmlParser-0.7.1 CEGUITGAImageCodec-0.7.1 CEGUIFalagardWRBase-0.7.1)


findpkg_finish(CEGUI)

