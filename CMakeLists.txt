
project(NickSVChat LANGUAGES C CXX)

include(GNUInstallDirs)
include(CheckSymbolExists)

cmake_minimum_required(VERSION 3.10)

option(BUILD_SHARED_LIBS ON)

set(GameNetworkingSockets_DIR "" CACHE STRING "Path where to find GameNetworkingSockets")

#add developer's path
set(GameNetworkingSockets_DIR_HINTS ${GameNetworkingSockets_DIR} "E:/SDK/GameNetworkingSockets/install")

#Protobuf is required for GameNetworkingSockets
find_package(Protobuf REQUIRED)

#GameNetworkingSockets is main network api for this project
find_package(GameNetworkingSockets REQUIRED
    HINTS ${GameNetworkingSockets_DIR_HINTS} 
)

if(GameNetworkingSockets_FOUND)
    message("-- Found GameNetworkingSockets")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#default character type setting
set(CHARACTER_TYPE "USE_WCHAR" CACHE STRING "C++ character type to use for messages, nicknames, etc.
                                                 Variants: 
                                                 USE_WCHAR  for wchar_t (default)
                                                 USE_CHAR   fot char
                                                 USE_CHAR16 for char16_t
                                                 USE_CHAR32 for char32_t"
                                                 )

if( (NOT CHARACTER_TYPE STREQUAL "USE_WCHAR") AND 
    (NOT CHARACTER_TYPE STREQUAL "USE_CHAR") AND 
    (NOT CHARACTER_TYPE STREQUAL "USE_CHAR16") AND 
    (NOT CHARACTER_TYPE STREQUAL "USE_CHAR32") )
     set(CHARACTER_TYPE "USE_WCHAR" CACHE STRING "" FORCE)
     message(WARNING "Invalid CHARACTER_TYPE was set, default (USE_WCHAR) is used")
endif()

#Compiler settings
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message("-- Found MSVC compiler")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}    ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}     ")
else()
    message("-- Found GNU compiler")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -static -static-libgcc -static-libstdc++ -lShlwapi")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CHARACTER_TYPE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCHAT_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


add_library(${PROJECT_NAME} 
    src/ChatClient.cpp 
    src/ChatServer.cpp
    src/ClientInfo.cpp
    src/Serializers.cpp
    )

    
set(NickSVChat_INCLUDE_DIR include)

target_include_directories(NickSVChat PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>  
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}/GameNetworkingSockets>
)

target_link_libraries(${PROJECT_NAME} 
    GameNetworkingSockets::GameNetworkingSockets
    )

install(
	TARGETS NickSVChat
	EXPORT NickSVChat
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
	EXPORT NickSVChat
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
	)

install(
    DIRECTORY ${NickSVChat_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/
    )

include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/NickSVChatConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/NickSVChatConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
	PATH_VARS CMAKE_INSTALL_FULL_INCLUDEDIR
	)

install(FILES 
	${CMAKE_CURRENT_BINARY_DIR}/NickSVChatConfig.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
	)