# - Try to find CEGUI
# Once done, this will define
#
#  CEGUI_FOUND - system has CEGUI
#  CEGUI_INCLUDE_DIRS - the CEGUI include directories 
#  CEGUI_LIBRARIES - link these to use CEGUI

include(FindPkgMacros)
findpkg_begin(CEGUI)

set(CEGUI_LIBRARY_NAMES
    CEGUIBase
    CEGUIFalagardWRBase
    CEGUIOpenGLRenderer
    CEGUILibxmlParser
)
get_debug_names(CEGUI_LIBRARY_NAMES)

use_pkgconfig(CEGUI_PKGC CEGUI)

find_path(CEGUI_INCLUDE_DIR NAMES CEGUI.h PATHS ${CEGUI_PKGC_INCLUDE_DIRS} ${CEGUI_INC_SEARCH_PATH} PATH_SUFFIXES "CEGUI")
find_library(CEGUI_LIBRARY_REL NAMES ${CEGUI_LIBRARY_NAMES} PATHS ${CEGUI_PKGC_LIBRARY_DIRS})
find_library(CEGUI_LIBRARY_DBG NAMES ${CEGUI_LIBRARY_NAMES_DBG} PATHS ${CEGUI_PKGC_LIBRARY_DIRS})
make_library_set(CEGUI_LIBRARY)

findpkg_finish(CEGUI)
add_parent_dir(CEGUI_INCLUDE_DIRS CEGUI_INCLUDE_DIR)
message(${CEGUI_LIBRARY_REL})

