
cmake_minimum_required(VERSION 3.16)

project(NickSVChat LANGUAGES C CXX VERSION 0.1.0)

set(GameNetworkingSockets_DIR "" CACHE STRING "Path where to find GameNetworkingSockets")
#added API author's path to GNS
if(WIN32)
    set(GameNetworkingSockets_DIR_HINTS ${GameNetworkingSockets_DIR} "E:/SDK/GameNetworkingSockets/install/Windows")
else()
    set(GameNetworkingSockets_DIR_HINTS ${GameNetworkingSockets_DIR} "/mnt/e/SDK/GameNetworkingSockets/install/Linux")
endif()

set(NickSVChat_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/include/)

#get_target_property(GameNetworkingSockets_INCLUDE_DIR GameNetworkingSockets::GameNetworkingSockets INTERFACE_INCLUDE_DIRECTORIES)
#message(STATUS "GameNetworkingSockets_INCLUDE_DIR: ${GameNetworkingSockets_INCLUDE_DIR}")

set(NickSVChat_EXTERNAL_INCLUDE_DIRS
    ${GameNetworkingSockets_INCLUDE_DIR}
    ${Protobuf_INCLUDE_DIR}
    )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
if(WIN32)
    set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL 3 CACHE STRING "If there are fewer dll/so files in the
                                                                    install bin directory than this variable, 
                                                                    dependencies installation will be skipped.")
else()
    set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL 2 CACHE STRING "Read description above ^^^")
endif()


#-----------------------------------------------
#OPTIONS
#-----------------------------------------------
include(CMakeDependentOption)
option(INSTALL_RUNTIME_DEPENDENCIES "Installing runtime deps to bin folder. Note: this is a custom script, so not all dependencies may be installed." OFF)
option(BUILD_SHARED_LIBS "Default type is SHARED" ON)
cmake_dependent_option(BUILD_TESTS "No tests build by default, but tests only build if INSTALL_RUNTIME_DEPENDENCIES set to ON" 
                        OFF "INSTALL_RUNTIME_DEPENDENCIES" OFF)
cmake_dependent_option(BUILD_EXAMPLES "No examples build by default, but examples only build if INSTALL_RUNTIME_DEPENDENCIES set to ON" 
                        OFF "INSTALL_RUNTIME_DEPENDENCIES" OFF)
set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL ${MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL_DEFAULT} CACHE STRING 
                                                                    "If there are fewer dll/so files in the
                                                                    install bin directory than this variable, 
                                                                    dependencies installation will be skipped.")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX 
    "${CMAKE_SOURCE_DIR}/install/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}" CACHE PATH "Default install path" FORCE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#-----------------------------------------------O
#-----------------------------------------------
#-----------------------------------------------






#-----------------------------------------------
#COMPILER SETTINGS
#-----------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c- /utf-8 /wd4146 /wd4530 /wd4244 /wd4251 /wd4267")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}    ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}     ")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -pedantic -Wall -Wextra -Wattributes -Wpedantic -Wconversion \
    -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization \
    -Wformat=2 -Winit-self -Wlogical-op \
    -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual \
    -Wredundant-decls -Wshadow -Wsign-promo \
    -Wstrict-null-sentinel -Wswitch-default -Wundef -Werror -Wno-unused"
    ) #-static -static-libgcc -static-libstdc++
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CHARACTER_TYPE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

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





