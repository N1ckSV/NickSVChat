
project(NickSVChat LANGUAGES CXX)

include(GNUInstallDirs)

cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

#GameNetworkingSockets depends on Protobuf
find_package(Protobuf REQUIRED)

#Trying to find GameNetworkingSockets (MAIN NETWORK LIB) in external folder or system dirs
find_package(GameNetworkingSockets REQUIRED CONFIG HINTS external)

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


add_library(${PROJECT_NAME} SHARED 
    src/ChatClient.cpp 
    src/ChatServer.cpp
    src/ClientInfo.cpp
    src/Serializers.cpp
    )


set(NickSVChat_INCLUDE_DIR include)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${NickSVChat_INCLUDE_DIR}
    )

target_link_libraries(${PROJECT_NAME} 
    GameNetworkingSockets::GameNetworkingSockets
    )


#install(
#    FILES external/bin/GameNetworkingSockets/${CMAKE_PLATFORM_NAME}/GameNetworkingSockets.dll 
#    DESTINATION ${CMAKE_INSTALL_BINDIR}
#    )
#
#install(
#    FILES external/bin/google/${CMAKE_PLATFORM_NAME}/libprotobuf.dll
#    DESTINATION ${CMAKE_INSTALL_BINDIR}
#    )
#
#install(
#    FILES external/bin/google/${CMAKE_PLATFORM_NAME}/libcrypto-3-x64.dll
#    DESTINATION ${CMAKE_INSTALL_BINDIR}
#    )



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
	${CMAKE_CURRENT_BINARY_DIR}/NickSVChat.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
	)