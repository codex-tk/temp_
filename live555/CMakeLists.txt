project(live555)

include_directories(    ${CMAKE_CURRENT_SOURCE_DIR}/BasicUsageEnvironment/include
                        ${CMAKE_CURRENT_SOURCE_DIR}/groupsock/include
                        ${CMAKE_CURRENT_SOURCE_DIR}/UsageEnvironment/include
                        ${CMAKE_CURRENT_SOURCE_DIR}/liveMedia/include )

file(GLOB_RECURSE HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/BasicUsageEnvironment/include/*.hh 
                            ${CMAKE_CURRENT_SOURCE_DIR}/groupsock/include/*.hh 
                            ${CMAKE_CURRENT_SOURCE_DIR}/UsageEnvironment/include/*.hh 
                            ${CMAKE_CURRENT_SOURCE_DIR}/liveMedia/include/*.hh )
                            
file(GLOB_RECURSE SRCS  ${CMAKE_CURRENT_SOURCE_DIR}/BasicUsageEnvironment/*.c* 
                        ${CMAKE_CURRENT_SOURCE_DIR}/groupsock//*.c*
                        ${CMAKE_CURRENT_SOURCE_DIR}/UsageEnvironment/*.c*
                        ${CMAKE_CURRENT_SOURCE_DIR}/liveMedia/*.c* )

MESSAGE( STATUS "HEADERS " ${HEADERS} )
MESSAGE( STATUS "SRCS " ${SRCS})

add_library(live555 STATIC ${HEADERS} ${SRCS})