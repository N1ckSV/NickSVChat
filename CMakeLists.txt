
cmake_minimum_required(VERSION 3.16)

project(NickSVChat LANGUAGES C CXX VERSION 0.1.0)

set(GameNetworkingSockets_DIR "" CACHE STRING "Path where to find GameNetworkingSockets")
#FIXME move next lines to private file
#author's path to GNS
set(GameNetworkingSockets_DIR_HINTS ${GameNetworkingSockets_DIR} $<IF:$<BOOL:WIN32>,
    "E:/SDK/GameNetworkingSockets/install/Windows" ,
    "/mnt/e/SDK/GameNetworkingSockets/install/Linux" >)


set(NickSVChat_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/include/)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#-----------------------------------------------
#OPTIONS
#-----------------------------------------------
include(CMakeDependentOption)
option(INSTALL_RUNTIME_DEPENDENCIES "Installing runtime deps to bin folder. Note: this is a custom script, so not all dependencies may be installed." OFF)
option(BUILD_SHARED_LIBS "Default type is STATIC" OFF)
cmake_dependent_option(BUILD_TESTS "No tests build by default, but tests only build if INSTALL_RUNTIME_DEPENDENCIES set to ON" 
                        OFF "INSTALL_RUNTIME_DEPENDENCIES" OFF)
cmake_dependent_option(BUILD_EXAMPLES "No examples build by default, but examples only build if INSTALL_RUNTIME_DEPENDENCIES set to ON" 
                        OFF "INSTALL_RUNTIME_DEPENDENCIES" OFF)
option(Protobuf_USE_STATIC_LIBS "Using static protobuf lib by default" ON)

message(STATUS "CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT: ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}")                                                                    
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX 
    "${CMAKE_SOURCE_DIR}/install/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}" CACHE PATH "Default install path" FORCE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------






#-----------------------------------------------
#COMPILER SETTINGS
#-----------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

message(STATUS)
message(STATUS "C++ Standart: ${CMAKE_CXX_STANDARD}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Make program: ${CMAKE_MAKE_PROGRAM}")
message(STATUS "C++ Make generator: ${CMAKE_GENERATOR}")
message(STATUS "C++ Make generator: ${CMAKE_GENERATOR}")
message(STATUS "C++ Make generator instance: ${CMAKE_GENERATOR_INSTANCE}")
message(STATUS "C++ Make generator platform: ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "C++ Make generator toolset: ${CMAKE_GENERATOR_TOOLSET}")
message(STATUS)
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------







#-----------------------------------------------
#Adding source directory
#-----------------------------------------------
add_subdirectory(src)
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------









#-----------------------------------------------
#TESTS AND EXAMPLES
#-----------------------------------------------
add_subdirectory(examples)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(include/NickSV/Tools/tests)
    add_subdirectory(tests)
endif()

#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------

