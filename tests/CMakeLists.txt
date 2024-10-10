
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_executable(
    NickSVChatTest
    test.cpp 
)


if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

    set_target_properties(NickSVChatTest
        PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(NickSVChatTest PRIVATE PROTOBUF_USE_DLLS)
    endif(BUILD_SHARED_LIBS)

endif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")




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
    PRIVATE 
        NickSVChat
)

add_test(
    NAME test_main COMMAND NickSVChatTest
    )

set_tests_properties(test_main 
    PROPERTIES 
    TIMEOUT 20
)

if(BUILD_SHARED_LIBS)
    set_tests_properties(test_main 
        PROPERTIES 
        WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
    )
endif()