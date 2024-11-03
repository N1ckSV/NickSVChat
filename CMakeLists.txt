
cmake_minimum_required(VERSION 3.16)

project(NickSVChat LANGUAGES C CXX VERSION 0.1.0)


#-----------------------------------------------
# SOME READ-ONLY SETTINGS
#-----------------------------------------------
set(NickSVChat_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set(NickSVChat_INCLUDE_PREFIX "/NickSV/Chat/")
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/local/cmake)
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------




#-----------------------------------------------
# OPTIONS
#-----------------------------------------------

set(GameNetworkingSockets_DIR "" CACHE STRING "Path where to find GameNetworkingSockets")

set(Protobuf_DIR "" CACHE STRING "Path where to find Protobuf")

set(OPENSSL_ROOT_DIR "" CACHE STRING "Path where to find OpenSSL")

option(INSTALL_RUNTIME_DEPENDENCIES "Installing runtime deps to bin folder. Note: this is a custom script, so not all dependencies may be installed." OFF)

option(BUILD_SHARED_LIBS "Default type is STATIC" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(Protobuf_USE_STATIC_LIBS "Using static protobuf lib by default" ON)

#FIXME remake it as protobuf did
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX 
    "${CMAKE_SOURCE_DIR}/install/${CMAKE_HOST_SYSTEM_NAME}/" CACHE PATH "Default install path" FORCE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------


#-----------------------------------------------
# FINDING STUFF
#-----------------------------------------------

get_property(GENERATOR_IS_MULTI_CONFIG_VAR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

#author's stuff
include(LocalConfig OPTIONAL)

if(BUILD_SHARED_LIBS)
    set(GNS_TARGET GameNetworkingSockets::shared)
else()   
    set(GNS_TARGET GameNetworkingSockets::static)
endif()

set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH}
                       ${GameNetworkingSockets_DIR}
                       ${Protobuf_DIR}
                       ${OPENSSL_ROOT_DIR}
                    )


#TODO git check for NickSV Tools
#TODO git installing openssl, protobuf, gns



#Protobuf is required
#FIXME with installing proj will probably need win PATH with protoc
find_package(Protobuf REQUIRED)

#GameNetworkingSockets is main network api for this project
find_package(GameNetworkingSockets CONFIG REQUIRED)



#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------






#-----------------------------------------------
# COMPILER SETTINGS
#-----------------------------------------------

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
# Adding source directory
#-----------------------------------------------
add_subdirectory(src)
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------









#-----------------------------------------------
# TESTS AND EXAMPLES
#-----------------------------------------------
add_subdirectory(examples)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------

