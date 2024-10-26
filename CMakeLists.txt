
cmake_minimum_required(VERSION 3.16)

project(NickSVChat LANGUAGES C CXX VERSION 0.1.0)

set(NickSVChat_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set(NickSVChat_INCLUDE_PREFIX "/NickSV/Chat/")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#-----------------------------------------------
#OPTIONS
#-----------------------------------------------

set(GameNetworkingSockets_DIR "" CACHE STRING "Path where to find GameNetworkingSockets")
set(OPENSSL_ROOT_DIR "" CACHE STRING "Path where to find OpenSSL")
option(INSTALL_RUNTIME_DEPENDENCIES "Installing runtime deps to bin folder. Note: this is a custom script, so not all dependencies may be installed." OFF)
option(BUILD_SHARED_LIBS "Default type is STATIC" OFF)
option(Protobuf_USE_STATIC_LIBS "Using static protobuf lib by default" ON)

include(CMakeDependentOption)
cmake_dependent_option(BUILD_TESTS "No tests build by default, but tests only build if INSTALL_RUNTIME_DEPENDENCIES set to ON" 
                        OFF "INSTALL_RUNTIME_DEPENDENCIES" OFF)
cmake_dependent_option(BUILD_EXAMPLES "No examples build by default, but examples only build if INSTALL_RUNTIME_DEPENDENCIES set to ON" 
                        OFF "INSTALL_RUNTIME_DEPENDENCIES" OFF)
                                                                   
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX 
    "${CMAKE_SOURCE_DIR}/install/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}" CACHE PATH "Default install path" FORCE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------



list(APPEND GameNetworkingSockets_DIR_HINTS ${GameNetworkingSockets_DIR})
#author's stuff
include(local/LocalConfig)



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

