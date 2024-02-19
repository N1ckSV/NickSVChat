
cmake_minimum_required(VERSION 3.10)

include(CTest)

add_executable(NickSVChatTest
    test.cpp 
    )

set_target_properties(NickSVChatTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
)

set(NickSVChat_INSTALL_FULL_BINDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
message(STATUS "NickSVChat_INSTALL_FULL_BINDIR: ${NickSVChat_INSTALL_FULL_BINDIR}")

if(NOT TARGET NickSVChat)
    message(FATAL_ERROR "NickSVChat was not found as a target")
endif()

#get_property(LIB_MAIN_FILE TARGET NickSVChat PROPERTY OUTPUT_NAME_DEBUG)
#message(STATUS "NickSVChat_INSTALL_FULL_BINDIR: ${NickSVChat_INSTALL_FULL_BINDIR}")
#message(STATUS "File to test: ${LIB_MAIN_FILE}")
#
#if(NOT EXISTS ${LIB_MAIN_FILE})
#    message(FATAL_ERROR "NickSVChat library doesn't exist")
#endif()

target_include_directories(NickSVChatTest
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    )

target_link_libraries(NickSVChatTest
        NickSVChat
    )

#install(PROGRAMS "$<TARGET_FILE:NickSVChatTest>" DESTINATION "${NickSVChat_INSTALL_FULL_BINDIR}")

add_test(
    NAME test_main COMMAND NickSVChatTest
    WORKING_DIRECTORY "${NickSVChat_INSTALL_FULL_BINDIR}"
    )