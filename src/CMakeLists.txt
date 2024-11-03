
include(GNUInstallDirs)
include(CheckSymbolExists)




macro(check_submodule SUBMODULE_PATH TESTFILE)
	if (NOT EXISTS ${TESTFILE} AND EXISTS "${CMAKE_SOURCE_DIR}/.git" )
		find_package(Git QUIET)
		if(GIT_FOUND)

			# Update submodules as needed
			message(STATUS "git submodule update --init ${SUBMODULE_PATH}")
			execute_process(COMMAND "${GIT_EXECUTABLE}" submodule update --init ${SUBMODULE_PATH}
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
							RESULT_VARIABLE GIT_SUBMOD_RESULT)
			if(NOT GIT_SUBMOD_RESULT EQUAL "0")
				message(FATAL_ERROR "'git submodule update --init ${SUBMODULE_PATH}' failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
			endif()
		endif()
	endif()
	if (NOT EXISTS ${TESTFILE})
		message(FATAL_ERROR "${TESTFILE} not found; submodule ${SUBMODULE_PATH} not initialized and we failed to initialized it.\nTry: 'git submodule update ${SUBMODULE_PATH}'" )
	endif()
endmacro()

set(NickSVTools_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules/Tools")
set(NickSVTools_INCLUDE_DIR "${NickSVTools_SOURCE_DIR}/include/")
check_submodule(${NickSVTools_SOURCE_DIR} "${NickSVTools_SOURCE_DIR}/include/NickSV/Tools/Definitions.h")
add_subdirectory(modules/Tools)
if(NOT TARGET NickSVTools)
    message(FATAL_ERROR "NickSVTools was not found as a target")
endif()


#-----------------------------------------------
#CONFIGURING LIBRARY
#-----------------------------------------------
configure_file(
    ../cmake/ChatConfig.h.in 
    ${NickSVChat_INCLUDE_DIR}/NickSV/Chat/ChatConfig.h
)



set(NickSVChat_SOURCE_FILES
    ChatSocket.cpp
    ChatClient.cpp
    ChatServer.cpp
    Request.cpp
    ClientInfo.cpp
    Message.cpp
    Utils.cpp
    Proto/ClientInfo.proto
    Proto/Message.proto
    Proto/Request.proto
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<BOOL:${BUILD_SHARED_LIBS}>:DLL>$<$<CONFIG:Debug>:Debug>")

if(BUILD_SHARED_LIBS)
    add_library(NickSVChat SHARED ${NickSVChat_SOURCE_FILES})
else()
    add_library(NickSVChat STATIC ${NickSVChat_SOURCE_FILES})
endif()

target_link_libraries(NickSVChat
    PUBLIC 
        ${GNS_TARGET} # shared or static
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


protobuf_generate (
    TARGET NickSVChat
    LANGUAGE cpp
    OUT_VAR NickSVChat_PROTO_OUT_FILES
    PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${NickSVChat_INCLUDE_PREFIX}
    )

    
set(NickSVChat_PROTO_OUT_DIRS)
set(NickSVChat_PROTO_HDRS ${NickSVChat_PROTO_OUT_FILES})

list(FILTER NickSVChat_PROTO_HDRS INCLUDE REGEX ".*\\.h$")

foreach(PROTO_FILE ${NickSVChat_PROTO_OUT_FILES})
    get_filename_component(PROTO_DIR ${PROTO_FILE} DIRECTORY)
    list(APPEND NickSVChat_PROTO_OUT_DIRS ${PROTO_DIR})
endforeach()
list(REMOVE_DUPLICATES NickSVChat_PROTO_OUT_DIRS)


target_include_directories(NickSVChat
PUBLIC
    "$<BUILD_INTERFACE:${NickSVChat_INCLUDE_DIR}>"
    "$<BUILD_INTERFACE:${NickSVTools_INCLUDE_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"                              #protobuf's headers
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${NickSVChat_INCLUDE_PREFIX}>" #protobuf's headers
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
        ${GNS_TARGET}
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
    DIRECTORY   
        ${NickSVChat_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/
)
file(TO_CMAKE_PATH "${CMAKE_INSTALL_FULL_INCLUDEDIR}/${NickSVChat_INCLUDE_PREFIX}/Proto" NickSVChat_PROTO_HDRS_DEST)
install(FILES 
	${NickSVChat_PROTO_HDRS}
	DESTINATION "${NickSVChat_PROTO_HDRS_DEST}"
	)
    
#-----------------------------------------------
#-----------------------------------------------
#-----------------------------------------------