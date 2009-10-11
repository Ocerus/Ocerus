#builds the rudeconfig library

cmake_minimum_required(VERSION 2.6)

set(SRC_FILES
    src/AbstractData.cpp
    src/AbstractParser.cpp
    src/WhiteSpace.cpp
    src/ParserJuly2004.cpp
    src/Section.cpp
    src/KeyValue.cpp
    src/RealOrganiser.cpp
    src/ConfigImpl.cpp
    src/DataLine.cpp
    src/Writer.cpp
    src/config.cpp
    src/AbstractOrganiser.cpp
    src/Base64Encoder.cpp
    src/Comment.cpp
    src/File.cpp
    src/AbstractWriter.cpp    
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/rudeconfig )

#link_directories( ${LINK_DIRS} )

add_library( rudeconfig ${SRC_FILES} )

#target_link_libraries( ois ${LIBS})

