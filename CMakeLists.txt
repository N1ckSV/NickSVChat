
cmake_minimum_required(VERSION 3.16)

project(NickSVChat LANGUAGES C CXX VERSION 0.1.0)

include(GNUInstallDirs)
include(CheckSymbolExists)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GameNetworkingSockets_DIR "" CACHE STRING "Path where to find GameNetworkingSockets")
#added API author's path to GNS
set(GameNetworkingSockets_DIR_HINTS ${GameNetworkingSockets_DIR} "E:/SDK/GameNetworkingSockets/install")
#Protobuf is required for GameNetworkingSockets
find_package(Protobuf REQUIRED)
#GameNetworkingSockets is main network api for this project
find_package(GameNetworkingSockets REQUIRED
    HINTS ${GameNetworkingSockets_DIR_HINTS} 
)
set(NickSVChat_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set(NickSVChat_EXTERNAL_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/external/GameNetworkingSockets/include/"
    )

set(NickSVChat_ALL_INCLUDE_DIRS "${NickSVChat_INCLUDE_DIR};${NickSVChat_EXTERNAL_INCLUDE_DIRS}")
message(STATUS "NickSVChat_ALL_INCLUDE_DIRS: ${NickSVChat_ALL_INCLUDE_DIRS}")
#get_target_property(GameNetworkingSockets_INCLUDEDIR GameNetworkingSockets::GameNetworkingSockets INTERFACE_INCLUDE_DIRECTORIES)
#message(STATUS "GameNetworkingSockets_INCLUDEDIR: ${GameNetworkingSockets_INCLUDEDIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
if(WIN32)
    set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL 3 CACHE STRING "If there are fewer dll/so files in the
                                                                    install bin directory than this variable, 
                                                                    dependencies installation will be skipped.")
else()
    set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL 1 CACHE STRING "Read description above ^^^")
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
set(CHARACTER_TYPE "USE_WCHAR" CACHE STRING "C++ character type to use for messages, nicknames, etc.
                                             Variants: 
                                             USE_WCHAR  for wchar_t (default)
                                             USE_CHAR   fot char
                                             USE_CHAR16 for char16_t
                                             USE_CHAR32 for char32_t")
if( (NOT ${CHARACTER_TYPE} STREQUAL "USE_WCHAR") AND 
    (NOT ${CHARACTER_TYPE} STREQUAL "USE_CHAR") AND 
    (NOT ${CHARACTER_TYPE} STREQUAL "USE_CHAR16") AND 
    (NOT ${CHARACTER_TYPE} STREQUAL "USE_CHAR32") )
     set(CHARACTER_TYPE "USE_WCHAR" CACHE STRING "" FORCE)
     message(WARNING "Invalid CHARACTER_TYPE is used, default (USE_WCHAR) was set")
endif()
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------




#-----------------------------------------------
#COMPILER SETTINGS
#-----------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}    ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}     ")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=c++11 \
    -pedantic -Wall -Wextra -Wattributes -Wpedantic -Wconversion \
    -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization \
    -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
    -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual \
    -Wredundant-decls -Wshadow -Wsign-promo \
    -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused"
    ) #-static -static-libgcc -static-libstdc++
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CHARACTER_TYPE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

message(STATUS "\n\n")
message(STATUS "C++ Standart: ${CMAKE_CXX_STANDARD}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Make program: ${CMAKE_MAKE_PROGRAM}")
message(STATUS "C++ Make generator: ${CMAKE_GENERATOR}")
message(STATUS "C++ Make generator: ${CMAKE_GENERATOR}")
message(STATUS "C++ Make generator instance: ${CMAKE_GENERATOR_INSTANCE}")
message(STATUS "C++ Make generator platform: ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "C++ Make generator toolset: ${CMAKE_GENERATOR_TOOLSET}")
message(STATUS "\n\n")
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------





#-----------------------------------------------
#CONFIGURING LIBRARY
#-----------------------------------------------
configure_file(cmake/ChatConfig.h.in include/NickSV/Chat/ChatConfig.h)
add_library(${PROJECT_NAME} 
    src/ChatSocket.cpp
    src/ChatClient.cpp
    src/ChatServer.cpp
    src/ClientInfo.cpp
    src/BasicMessage.cpp
    src/Serializers.cpp
    src/Parsers.cpp
    src/Requests.cpp
    src/Utils.cpp
    )
target_include_directories(NickSVChat PUBLIC
    "$<BUILD_INTERFACE:${NickSVChat_INCLUDE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(NickSVChat SYSTEM PUBLIC
    "$<BUILD_INTERFACE:${NickSVChat_EXTERNAL_INCLUDE_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${PROJECT_NAME} 
    GameNetworkingSockets::GameNetworkingSockets
)
#set_target_properties(NickSVChat PROPERTIES LINK_FLAGS "-static -static-libgcc -static-libstdc++")
#-----------------------------------------------
#-----------------------------------------------
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
    DIRECTORY   ${NickSVChat_ALL_INCLUDE_DIRS}
    DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/"
)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(include/NickSV/Tools/tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------