cmake_minimum_required(VERSION 3.10)


add_executable(NickSVChatExample
    ConsoleChat.cpp 
    )

set_target_properties(NickSVChatExample
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/examples"
)

set(NickSVChat_INSTALL_FULL_BINDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
message(STATUS "NickSVChat_INSTALL_FULL_BINDIR: ${NickSVChat_INSTALL_FULL_BINDIR}")

if(NOT TARGET NickSVChat)
    message(FATAL_ERROR "NickSVChat was not found as a target")
endif()


target_include_directories(NickSVChatExample
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    )

if(WIN32)
    target_link_libraries(NickSVChatExample
        NickSVChat
    )
else()
    target_link_libraries(NickSVChatExample
        NickSVChat
    )
endif()

#set_target_properties(NickSVChatExample PROPERTIES LINK_FLAGS "-static-libstdc++")

install(PROGRAMS "$<TARGET_FILE:NickSVChatExample>" DESTINATION "${NickSVChat_INSTALL_FULL_BINDIR}")
