project(tlab.test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tlab/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../live555/BasicUsageEnvironment/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../live555/groupsock/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../live555/UsageEnvironment/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../live555/liveMedia/include )

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )

add_executable(tlab.test ${SRCS})

find_package(ZLIB)
if (ZLIB_FOUND)
    target_link_libraries(tlab.test ${ZLIB_LIBRARIES})
endif(ZLIB_FOUND)

if(Boost_FOUND)
    target_link_libraries(tlab.test ${Boost_LIBRARIES})
endif()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../sqlite3/include )
add_definitions("-DTLAB_ENABLE_SQLITE3_DB")

MESSAGE( STATUS "TEST SRCS " ${SRCS} )

target_link_libraries(tlab.test tlab sqlite3 live555 gtest)
if(WIN32)
    target_link_libraries(tlab.test ws2_32)
else()
    target_link_libraries(tlab.test dl)
endif()

enable_testing()
add_test(tlab.test tlab.test)
add_custom_command(TARGET tlab.test POST_BUILD COMMAND ctest -C $<CONFIGURATION> --verbose --output-on-failure )
