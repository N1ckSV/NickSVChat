
# This script ( use install_target_deps_recursive function )
# adding INSTALL CODE for runtime dependencies of given targets






cmake_minimum_required(VERSION 3.20)

include(GNUInstallDirs)

macro(add_dir_if_exist_macro _INNER_LIST_TO_ADD _IF_EXIST_VAR)
    if(IS_DIRECTORY ${_IF_EXIST_VAR})
        list(APPEND ${_INNER_LIST_TO_ADD} ${_IF_EXIST_VAR})
        message(STATUS "[ITDR] -- DIR EXISTS: ${_IF_EXIST_VAR}")
    else()
        message(STATUS "[ITDR] -- NO DIR EXISTS: ${_IF_EXIST_VAR}")
    endif()
endmacro()




macro(target_add_imported_property_if_exist TGT _LIST_TO_ADD_FILES _LIST_TO_ADD_DIRS)
    get_target_property(TGT_TYPE ${TGT} TYPE)
    message(STATUS "[ITDR] -- ${TGT}'s TYPE is ${TGT_TYPE}")
    if(${TGT_TYPE} STREQUAL "SHARED_LIBRARY" OR ${TGT_TYPE} STREQUAL "UNKNOWN_LIBRARY")

        foreach(PROP IN ITEMS IMPORTED_LOCATION_RELEASE IMPORTED_LOCATION_DEBUG IMPORTED_LOCATION)

            get_target_property(_IMPORTED_LOCATION_VAR ${TGT} ${PROP})
            if(_IMPORTED_LOCATION_VAR)

                message(STATUS "[ITDR] -- PROPERTY ${PROP} OF TARGET ${TGT} IS FOUND: ${_IMPORTED_LOCATION_VAR}")
                if(${TGT_TYPE} STREQUAL "SHARED_LIBRARY")
                    list(APPEND ${_LIST_TO_ADD_FILES} ${_IMPORTED_LOCATION_VAR})
                endif(${TGT_TYPE} STREQUAL "SHARED_LIBRARY")
                get_filename_component(_IMPORTED_LOCATION_DIR ${_IMPORTED_LOCATION_VAR} DIRECTORY REALPATH)
                if(WIN32)
                    set(_IMPORTED_LOCATION_DIR ${_IMPORTED_LOCATION_DIR}/../bin)
                elseif(UNIX)
                    set(_IMPORTED_LOCATION_DIR ${_IMPORTED_LOCATION_DIR}/../lib)
                endif()
                cmake_path(SET _IMPORTED_LOCATION_DIR NORMALIZE ${_IMPORTED_LOCATION_DIR})
                message(STATUS "[ITDR] -- CHECKING FOR BIN DIR...")
                add_dir_if_exist_macro(${_LIST_TO_ADD_DIRS} ${_IMPORTED_LOCATION_DIR})
                break()

            else(_IMPORTED_LOCATION_VAR)
                message(STATUS "[ITDR] -- NO PROPERTY ${PROP} OF TARGET ${TGT} IS FOUND")
            endif(_IMPORTED_LOCATION_VAR)

        endforeach()
        
        list(REMOVE_DUPLICATES ${_LIST_TO_ADD_FILES})
        list(REMOVE_DUPLICATES ${_LIST_TO_ADD_DIRS})

    endif(${TGT_TYPE} STREQUAL "SHARED_LIBRARY" OR ${TGT_TYPE} STREQUAL "UNKNOWN_LIBRARY")
endmacro()

#[===[
 get_all_runtime_deps_and_dirs(<list-files-var> <list-dirs-var> <targets>...)
 Finding whole accessible dependency tree.
 If target is shared lib - adding runtime file to <list-files-var>.
 If target is unknown lib and <path-to-target-lib>/bin exists - adding this dir to <list-dirs-var>.
 Recursive for each TARGET with <targets> = [TARGET's linked libs]
 ]===]
macro(get_all_runtime_deps_and_dirs _LIST_FILES_TO_ADD _LIST_DIRS_TO_ADD)
    foreach(TGT ${ARGN})

        if(TARGET ${TGT})

            get_target_property(TGT_TYPE ${TGT} TYPE)
            target_add_imported_property_if_exist(${TGT} ${_LIST_FILES_TO_ADD} ${_LIST_DIRS_TO_ADD})
            get_target_property(_LINKED_LIBS ${TGT} INTERFACE_LINK_LIBRARIES)
            if(_LINKED_LIBS)
                get_all_runtime_deps_and_dirs(${_LIST_FILES_TO_ADD} ${_LIST_DIRS_TO_ADD} ${_LINKED_LIBS})
            endif(_LINKED_LIBS)

        endif(TARGET ${TGT})

    endforeach()
endmacro()


function(install_target_deps_recursive TGT)
    message(STATUS "[ITDR] -- ITDR is InstallTargetDepsRecursive script")
    if(WIN32 OR UNIX)
        get_all_runtime_deps_and_dirs(LIST_FILES_TO_ADD_VAR LIST_DIRS_TO_ADD_VAR ${ARGN})
        
        list(REMOVE_DUPLICATES LIST_FILES_TO_ADD_VAR)
        list(REMOVE_DUPLICATES LIST_DIRS_TO_ADD_VAR)

        message(STATUS "[ITDR] -- Target's dep files: ${LIST_FILES_TO_ADD_VAR}")
        message(STATUS "[ITDR] -- Target's dep dirs: ${LIST_DIRS_TO_ADD_VAR}")

        #Transfer paths to install config
        install(CODE "message(STATUS \"[ITDR] -- ITDR is InstallTargetDepsRecursive script\")")
        install(CODE "set(LIST_FILES_TO_ADD_VAR_TRANSFER \"$<TARGET_FILE:${TGT}>;${LIST_FILES_TO_ADD_VAR}\")")
        install(CODE "set(LIST_DIRS_TO_ADD_VAR_TRANSFER \"${LIST_DIRS_TO_ADD_VAR}\")")
        
        install(CODE "list(REMOVE_DUPLICATES LIST_FILES_TO_ADD_VAR_TRANSFER)")
        install(CODE "list(REMOVE_DUPLICATES LIST_DIRS_TO_ADD_VAR_TRANSFER)")

        install(CODE "list(LENGTH LIST_FILES_TO_ADD_VAR_TRANSFER LIST_FILES_TO_ADD_VAR_TRANSFER_LENGTH)")
        
        if(WIN32)
            install(CODE "set(INSTALL_SHARED_LIBDIR_TRANSFER \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\")")
        elseif(UNIX)
            install(CODE "set(INSTALL_SHARED_LIBDIR_TRANSFER \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\")")
        endif()

        # Also install shared library dependencies
        install(CODE  [[
            if(WIN32)
                file(GLOB _RUNTIME_FILES_TO_CHECK_LIST "${INSTALL_SHARED_LIBDIR_TRANSFER}/*.dll")
            elseif(UNIX)
                file(GLOB _RUNTIME_FILES_TO_CHECK_LIST "${INSTALL_SHARED_LIBDIR_TRANSFER}/*.so")
            endif()
            list(LENGTH _RUNTIME_FILES_TO_CHECK_LIST _RUNTIME_FILES_TO_CHECK_LIST_LENGTH)
            message(STATUS "[ITDR] -- INSTALL_SHARED_LIBDIR_TRANSFER: ${INSTALL_SHARED_LIBDIR_TRANSFER}")
            message(STATUS "[ITDR] -- RUNTIME_FILES_TO_CHECK_LIST: ${_RUNTIME_FILES_TO_CHECK_LIST}")
            if(_RUNTIME_FILES_TO_CHECK_LIST_LENGTH GREATER_EQUAL ${LIST_FILES_TO_ADD_VAR_TRANSFER_LENGTH})
                message(WARNING "[ITDR] -- There are enough runtime files found in the installation binary folder so no installation required, but you are able to set MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL_TRANSFER variable to change the skip threshold")
            else()
                if(WIN32)
                    file(GET_RUNTIME_DEPENDENCIES
                        LIBRARIES
                            ${LIST_FILES_TO_ADD_VAR_TRANSFER}
                        RESOLVED_DEPENDENCIES_VAR _r_deps
                        UNRESOLVED_DEPENDENCIES_VAR _u_deps
                        DIRECTORIES
                            ${LIST_DIRS_TO_ADD_VAR_TRANSFER}
                    )
                elseif(UNIX)
                    set(_r_deps ${LIST_FILES_TO_ADD_VAR_TRANSFER})
                endif()
                foreach(_file ${_r_deps})
                    string(TOLOWER ${_file} _file_lower)
                    set(_IS_SYSTEM_FILE FALSE)

                    if(WIN32)
                        if ((${_file_lower} MATCHES "^c:[\\/]windows[\\/]system32.*")
                         OR (${_file_lower} MATCHES "^c:[\\/]windows[\\/]syswow64.*"))
                            set(_IS_SYSTEM_FILE TRUE)
                        endif()
                    elseif(UNIX)
                        if ((${_file_lower} MATCHES "^/lib.*")
                         OR (${_file_lower} MATCHES "^/usr/lib.*")
                         OR (${_file_lower} MATCHES "^/lib64.*")
                         OR (${_file_lower} MATCHES "^/usr/lib64.*"))
                            set(_IS_SYSTEM_FILE TRUE)
                        endif()
                    endif()

                    if(_IS_SYSTEM_FILE)
                        if(NOT EXISTS ${_file})
                            get_filename_component(_FILE_NAME ${_file} NAME)
                            foreach(_dir ${LIST_DIRS_TO_ADD_VAR_TRANSFER})
                                if(EXISTS "${_dir}/${_FILE_NAME}")
                                    file(INSTALL
                                    DESTINATION "${INSTALL_SHARED_LIBDIR_TRANSFER}/"
                                    TYPE SHARED_LIBRARY
                                    FOLLOW_SYMLINK_CHAIN
                                    FILES "${_dir}/${_FILE_NAME}")
                                    list(APPEND _r_deps_not_system32 "${_dir}/${_FILE_NAME}")
                                endif()
                            endforeach()
                        endif(NOT EXISTS ${_file})
                    else()
                        message(STATUS "[ITDR] -- _file: ${_file}")
                        file(INSTALL
                            DESTINATION "${INSTALL_SHARED_LIBDIR_TRANSFER}/"
                            TYPE SHARED_LIBRARY
                            FOLLOW_SYMLINK_CHAIN
                            FILES "${_file}"
                        )
                        list(APPEND _r_deps_not_system32 ${_file})
                    endif()
                endforeach()
                message(VERBOSE "[ITDR] -- RESOLVED_DEPENDENCIES_VAR for target ${TGT}: ${_r_deps_not_system32}")
                
                message(VERBOSE "[ITDR] -- UNRESOLVED_DEPENDENCIES_VAR for target ${TGT}: ${_u_deps}")
            endif(_RUNTIME_FILES_TO_CHECK_LIST_LENGTH GREATER_EQUAL ${MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL_TRANSFER})

        ]])
    else(WIN32 OR UNIX)
        message(WARNING "[ITDR] -- InstallTargetDepsRecursive script is only implemented for Windows and Unix, no action taken")
    endif(WIN32 OR UNIX)
endfunction()