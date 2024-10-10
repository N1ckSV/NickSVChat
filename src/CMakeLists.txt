
include(GNUInstallDirs)
include(CheckSymbolExists)


#Protobuf is required
find_package(Protobuf REQUIRED)
#GameNetworkingSockets is main network api for this project
find_package(GameNetworkingSockets REQUIRED
    HINTS ${GameNetworkingSockets_DIR_HINTS} 
)

get_target_property(GameNetworkingSockets_INCLUDE_DIR GameNetworkingSockets::static INTERFACE_INCLUDE_DIRECTORIES)

set(NickSVChat_EXTERNAL_INCLUDE_DIRS
    ${GameNetworkingSockets_INCLUDE_DIR}
    ${Protobuf_INCLUDE_DIR}
    )
message(STATUS "NickSVChat_EXTERNAL_INCLUDE_DIRS: ${NickSVChat_EXTERNAL_INCLUDE_DIRS}")


#-----------------------------------------------
#CONFIGURING LIBRARY
#-----------------------------------------------
configure_file(
    ../cmake/ChatConfig.h.in 
    ${NickSVChat_INCLUDE_DIR}/NickSV/Chat/ChatConfig.h
)


protobuf_generate_cpp(
    NickSVChat_PROTO_SRCS
    NickSVChat_PROTO_HDRS
    Proto/ClientInfo.proto
    Proto/Message.proto
    Proto/Request.proto
)

set(NickSVChat_PROTO_OUT_DIRS)
foreach(PROTO_FILE ${NickSVChat_PROTO_HDRS})
    get_filename_component(PROTO_DIR ${PROTO_FILE} DIRECTORY)
    list(APPEND NickSVChat_PROTO_OUT_DIRS ${PROTO_DIR})
endforeach()
list(REMOVE_DUPLICATES NickSVChat_PROTO_OUT_DIRS)

message(STATUS "NickSVChat_PROTO_HDRS: ${NickSVChat_PROTO_HDRS}")
message(STATUS "NickSVChat_PROTO_SRCS: ${NickSVChat_PROTO_SRCS}")
message(STATUS "NickSVChat_PROTO_OUT_DIRS: ${NickSVChat_PROTO_OUT_DIRS}")


set(NickSVChat_SOURCE_FILES
    ChatSocket.cpp
    ChatClient.cpp
    ChatServer.cpp
    Request.cpp
    ClientInfo.cpp
    Message.cpp
    Utils.cpp
    ${NickSVChat_PROTO_SRCS}
)


set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<BOOL:${BUILD_SHARED_LIBS}>:DLL>$<$<CONFIG:Debug>:Debug>")

if(BUILD_SHARED_LIBS)
    add_library(NickSVChat SHARED ${NickSVChat_SOURCE_FILES})
else()
    add_library(NickSVChat STATIC ${NickSVChat_SOURCE_FILES})
endif()


target_link_libraries(NickSVChat
    PRIVATE 
        GameNetworkingSockets::static
        protobuf::libprotobuf
)


if( BUILD_SHARED_LIBS AND
    CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
		target_compile_definitions(NickSVChat PRIVATE PROTOBUF_USE_DLLS)
else()
    #target_compile_definitions(NickSVChat PRIVATE PROTOBUF_USE_DLLS)
endif()


target_compile_definitions(NickSVChat
    PUBLIC
        STEAMNETWORKINGSOCKETS_OPENSOURCE
    PRIVATE
        NICKSVCHAT_FOREXPORT
)


target_include_directories(NickSVChat
PUBLIC
    "$<BUILD_INTERFACE:${NickSVChat_INCLUDE_DIR}>"
    "$<BUILD_INTERFACE:${NickSVChat_PROTO_OUT_DIRS}>"
INTERFACE
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(NickSVChat 
SYSTEM 
PUBLIC
    "$<BUILD_INTERFACE:${NickSVChat_EXTERNAL_INCLUDE_DIRS}>"
INTERFACE
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)


#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------


#-----------------------------------------------
#INSTALLING
#-----------------------------------------------
install(
	TARGETS NickSVChat
	EXPORT NickSVChat
	LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
)
install(
    EXPORT NickSVChat
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
)

if(INSTALL_RUNTIME_DEPENDENCIES)
    include(InstallTargetDepsRecursive)
    install_target_deps_recursive(
        NickSVChat
        GameNetworkingSockets::GameNetworkingSockets
        protobuf::libprotobuf
    )
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(../cmake/NickSVChatConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/NickSVChatConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
	)
install(FILES 
	${CMAKE_CURRENT_BINARY_DIR}/NickSVChatConfig.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NickSVChat
	)
install(
    DIRECTORY   ${NickSVChat_INCLUDE_DIR}
    DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/
)
    
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------