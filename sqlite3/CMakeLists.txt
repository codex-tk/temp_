project(sqlite3)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/sqlite3/ )

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c )

MESSAGE( STATUS "HEADERS " ${HEADERS} )
MESSAGE( STATUS "SRCS " ${SRCS} )

add_library(sqlite3 STATIC ${HEADERS} ${SRCS})