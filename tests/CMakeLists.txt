
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_executable(
    NickSVChatTest
    test.cpp 
)

set_target_properties(NickSVChatTest
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
)

if(NOT TARGET NickSVChat)
    message(FATAL_ERROR "NickSVChat was not found as a target")
endif()


target_link_libraries(NickSVChatTest
    NickSVChat
)

add_test(
    NAME test_main COMMAND NickSVChatTest
    )

set_tests_properties(test_main 
    PROPERTIES 
    TIMEOUT 10
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
)