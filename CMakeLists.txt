
project(NickSVChat LANGUAGES C CXX)

include(GNUInstallDirs)
include(CheckSymbolExists)

cmake_minimum_required(VERSION 3.24)

set(GameNetworkingSockets_DIR "" CACHE STRING "Path where to find GameNetworkingSockets")

#added API author's path to GNS
set(GameNetworkingSockets_DIR_HINTS ${GameNetworkingSockets_DIR} "E:/SDK/GameNetworkingSockets/install")

#Protobuf is required for GameNetworkingSockets
find_package(Protobuf REQUIRED)

#GameNetworkingSockets is main network api for this project
find_package(GameNetworkingSockets REQUIRED
    HINTS ${GameNetworkingSockets_DIR_HINTS} 
)

set(NickSVChat_INCLUDE_DIR include)

get_target_property(GameNetworkingSockets_INCLUDEDIR GameNetworkingSockets::GameNetworkingSockets INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "GameNetworkingSockets_INCLUDEDIR: ${GameNetworkingSockets_INCLUDEDIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CMakeDependentOption)



#-----------------------------------------------
#OPTIONS
#-----------------------------------------------
option(INSTALL_RUNTIME_DEPENDENCIES "Installing runtime deps to bin folder. Note: this is a custom script, so not all dependencies may be installed." OFF)
option(BUILD_SHARED_LIBS "Default type is SHARED" ON)
cmake_dependent_option(BUILD_TESTS "No tests build by default, but tests only builds if INSTALL_RUNTIME_DEPENDENCIES set to ON" 
                        OFF "INSTALL_RUNTIME_DEPENDENCIES" OFF)
option(BUILD_TESTS "No tests build by default" OFF)
if(WIN32)
    set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL 3 CACHE STRING "If there are fewer dll/so files in the
                                                                    install bin directory than this variable, 
                                                                    dependencies installation will be skipped.")
else()
    set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL 1 CACHE STRING "If there are fewer dll/so files in 
                                                                    the install bin directory than this variable, 
                                                                    dependencies installation will be skipped.")
endif()
#default character type setting
set(CHARACTER_TYPE "USE_WCHAR" CACHE STRING "C++ character type to use for messages, nicknames, etc.
                                                 Variants: 
                                                 USE_WCHAR  for wchar_t (default)
                                                 USE_CHAR   fot char
                                                 USE_CHAR16 for char16_t
                                                 USE_CHAR32 for char32_t"
                                                 )

if( (NOT ${CHARACTER_TYPE} STREQUAL "USE_WCHAR") AND 
    (NOT ${CHARACTER_TYPE} STREQUAL "USE_CHAR") AND 
    (NOT ${CHARACTER_TYPE} STREQUAL "USE_CHAR16") AND 
    (NOT ${CHARACTER_TYPE} STREQUAL "USE_CHAR32") )
     set(CHARACTER_TYPE "USE_WCHAR" CACHE STRING "" FORCE)
     message(WARNING "Invalid CHARACTER_TYPE is used, default (USE_WCHAR) was set")
endif()
#-----------------------------------------------
#
#-----------------------------------------------




#-----------------------------------------------
#COMPILER SETTINGS
#-----------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message("-- Found MSVC compiler")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}    ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}     ")
else()
    message("-- Found GNU compiler")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") #-static -static-libgcc -static-libstdc++
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CHARACTER_TYPE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCHAT_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
#-----------------------------------------------
#
#-----------------------------------------------





#-----------------------------------------------
#CONFIGURING LIBRARY
#-----------------------------------------------
add_library(${PROJECT_NAME} 
    src/Chat.cpp
    src/ClientInfo.cpp
    src/Serializers.cpp
    )

target_include_directories(NickSVChat PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${PROJECT_NAME} 
    GameNetworkingSockets::GameNetworkingSockets
)
#-----------------------------------------------
#
#-----------------------------------------------






#-----------------------------------------------
#INSTALLING
#-----------------------------------------------
install(
	TARGETS NickSVChat
	EXPORT NickSVChat
	LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
)

install(
    EXPORT NickSVChat
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat"
)

if(INSTALL_RUNTIME_DEPENDENCIES)
    include(InstallTargetDepsRecursive)
    install_target_deps_recursive(
        NickSVChat
        GameNetworkingSockets::GameNetworkingSockets
    )
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/NickSVChatConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/NickSVChatConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
	)

install(FILES 
	"${CMAKE_CURRENT_BINARY_DIR}/NickSVChatConfig.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat"
	)

install(
    DIRECTORY   "${NickSVChat_INCLUDE_DIR}/"
    DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/"
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()