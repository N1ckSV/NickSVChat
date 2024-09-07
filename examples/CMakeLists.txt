

if( BUILD_EXAMPLES )

    add_executable(
        NickSVChatExample
        ConsoleChat.cpp 
    )

    #set_target_properties(NickSVChatExample
    #    PROPERTIES
    #    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/examples"
    #)

    target_link_libraries(NickSVChatExample
        NickSVChat
    )

    install(
        PROGRAMS 
            "$<TARGET_FILE:NickSVChatExample>"
        DESTINATION 
           "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
    )

endif()