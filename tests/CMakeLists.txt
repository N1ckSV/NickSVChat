
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(NickSVChatTest LANGUAGES CXX)

include(GNUInstallDirs)

cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -static -static-libgcc -static-libstdc++")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DNICKSVCHAT_DEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_executable(${PROJECT_NAME} 
    test.cpp 
    )

SET(NickSVChatTest_INCLUDE_DIRS /../include)

message("-- NickSVChatTest_INCLUDE_DIRS = ${NickSVChatTest_INCLUDE_DIRS}")

target_include_directories(${PROJECT_NAME}
    PUBLIC ${NickSVChatTest_INCLUDE_DIRS}
    PUBLIC external/GameNetworkingSockets/include
    )

SET(NickSVChatTest_LINK_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_PLATFORM_NAME}/${CMAKE_BUILD_TYPE})

message("-- NickSVChatTest_LINK_DIRS = ${NickSVChatTest_LINK_DIRS}")

target_link_directories(${PROJECT_NAME} 
    PUBLIC ${NickSVChatTest_LINK_DIRS}
    )

target_link_libraries(${PROJECT_NAME} 
    ${NickSVChatTest_LINK_DIRS}/libNickSVChat.dll
    )

file(
    INSTALL ../external/bin/GameNetworkingSockets/${CMAKE_PLATFORM_NAME}/GameNetworkingSockets.dll 
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

file(
    INSTALL ../external/bin/google/${CMAKE_PLATFORM_NAME}/libprotobuf.dll
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

file(
    INSTALL ../external/bin/google/${CMAKE_PLATFORM_NAME}/libcrypto-3-x64.dll
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

file(
    INSTALL ${NickSVChatTest_LINK_DIRS}/libNickSVChat.dll
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
