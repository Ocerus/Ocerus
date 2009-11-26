#builds the SOIL library

cmake_minimum_required(VERSION 2.6)

set(SRC_FILES
    src/SOIL.c
    src/image_helper.c
    src/image_DXT.c
    src/stb_image_aug.c
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library( SOIL ${SRC_FILES} )

