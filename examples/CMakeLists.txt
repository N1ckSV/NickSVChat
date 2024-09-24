

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

    #cd ../../../.. ; cmake . --preset linux-debug-ninja-test;  cmake --build --preset linux-debug-ninja-test ; cd "./install/Linux/Debug/bin" ; ldd ./NickSVChatExample
    add_executable(
        NickSVChatExample
        ConsoleChat.cpp
        ${NickSVChatExample_PROTO_SRCS}
    )

    #set_target_properties(NickSVChatExample PROPERTIES
    #    SKIP_BUILD_RPATH FALSE
    #    BUILD_WITH_INSTALL_RPATH FALSE
    #    INSTALL_RPATH_USE_LINK_PATH FALSE
    #    INSTALL_RPATH "$ORIGIN/../lib"
    #)

    
    target_include_directories(NickSVChatExample
    PUBLIC
        ${NickSVChatExample_PROTO_OUT_DIRS}
    )

    #set_target_properties(NickSVChatExample
    #    PROPERTIES
    #    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/examples"
    #)

    target_link_libraries(NickSVChatExample
        NickSVChat
        protobuf::libprotobuf
    )

    install(TARGETS
        NickSVChatExample
        DESTINATION 
           "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
    )

endif()