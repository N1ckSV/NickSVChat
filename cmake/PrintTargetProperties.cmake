# This script ( use FindPackageAndBindTarget function )
# finding package as simple find_package and copying
# all of it information to first variable





cmake_minimum_required(VERSION 3.10)

# getting all existing props
if(NOT CMAKE_PROPERTY_LIST)
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
    
    # Convert command output into a CMake list
    string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
endif()

# printing all existing props
function(PrintProperties)
    message("CMAKE_PROPERTY_LIST = ${CMAKE_PROPERTY_LIST}")
endfunction()



# printing all props of given target
function(PrintTargetProperties target)

    if(NOT TARGET ${target})
        message(FATAL_ERROR "There is no target named '${target}'")
    endif()

    
    get_target_property(INITIAL_IMPORTED_CONFIGURATIONS ${target} IMPORTED_CONFIGURATIONS)
    get_target_property(IS_IMPORTED_VAR ${target} IMPORTED)

    foreach(property ${CMAKE_PROPERTY_LIST})

            if(property STREQUAL "LOCATION" OR
               property MATCHES "^LOCATION_" OR
               property MATCHES "_LOCATION$")
                if(NOT IS_IMPORTED_VAR)
                    continue()
                endif()
            endif()

            if(property MATCHES "<[Cc][Oo][Nn][Ff][Ii][Gg]>")
                foreach(config ${INITIAL_IMPORTED_CONFIGURATIONS})
                    string(REGEX REPLACE "<[Cc][Oo][Nn][Ff][Ii][Gg]>" "${config}" property_config ${property})
                    get_property(was_set TARGET ${target} PROPERTY ${property_config} SET)
                    if(was_set)
                        get_target_property(value ${target} ${property_config})
                        message("${target} ${property_config} = ${value}")
                    endif()
                endforeach(config ${INITIAL_IMPORTED_CONFIGURATIONS})
                continue()
            endif()

            get_property(was_set TARGET ${target} PROPERTY ${property} SET)
            if(was_set)
                get_target_property(value ${target} ${property})
                message("${target} ${property} = ${value}")
            endif()

    endforeach(property ${CMAKE_PROPERTY_LIST})

endfunction()