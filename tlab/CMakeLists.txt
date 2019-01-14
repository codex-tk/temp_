project(tlab)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp )
file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

MESSAGE( STATUS "HEADERS " ${HEADERS} )
MESSAGE( STATUS "SRCS " ${SRCS} )

add_definitions("-DTLAB_VERSION_STRING=\"TLIB 0.1\"")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../sqlite3/include )
add_definitions("-DTLAB_ENABLE_SQLITE3_DB")

add_library(tlab STATIC ${HEADERS} ${SRCS})