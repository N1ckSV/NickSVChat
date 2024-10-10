

if( BUILD_EXAMPLES )
    

    find_package(Protobuf REQUIRED) 

    protobuf_generate_cpp(
        NickSVChatExample_PROTO_SRCS
        NickSVChatExample_PROTO_HDRS
        UserName.proto
    )

    
    set(NickSVChatExample_PROTO_OUT_DIRS)
    foreach(PROTO_FILE ${NickSVChatExample_PROTO_HDRS})
        get_filename_component(PROTO_DIR ${PROTO_FILE} DIRECTORY)
        list(APPEND NickSVChatExample_PROTO_OUT_DIRS ${PROTO_DIR})
    endforeach()
    list(REMOVE_DUPLICATES NickSVChatExample_PROTO_OUT_DIRS)

    if(BUILD_SHARED_LIBS)
        # use, i.e. don't skip the full RPATH for the build tree
        set(CMAKE_SKIP_BUILD_RPATH  OFF)

        # when building, don't use the install RPATH already
        # (but later on when installing)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF) 

        # don't add the automatically determined parts of the RPATH
        # which point to directories outside the build tree to the install RPATH
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)

        #set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

        # the RPATH to be used when installing
        set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib") 
    endif(BUILD_SHARED_LIBS)
    
    add_executable(
        NickSVChatExample
        ConsoleChat.cpp
        ${NickSVChatExample_PROTO_SRCS}
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    
        set_target_properties(NickSVChatExample
            PROPERTIES
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        )

        if(BUILD_SHARED_LIBS)
		    target_compile_definitions(NickSVChatExample PRIVATE PROTOBUF_USE_DLLS)
        endif(BUILD_SHARED_LIBS)

    endif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

    
    target_include_directories(NickSVChatExample
        PUBLIC
            ${NickSVChatExample_PROTO_OUT_DIRS}
    )

    target_link_libraries(NickSVChatExample
        PRIVATE
            NickSVChat
        PRIVATE 
            protobuf::libprotobuf
    )


    if(BUILD_SHARED_LIBS)
        install(TARGETS
            NickSVChatExample
            DESTINATION 
               "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
        )
    endif(BUILD_SHARED_LIBS)

endif()