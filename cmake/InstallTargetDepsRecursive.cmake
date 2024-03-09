cmake_minimum_required(VERSION 3.20)

include(GNUInstallDirs)

macro(add_if_exist_macro _INNER_LIST_TO_ADD _IF_EXIST_VAR)
    if(IS_DIRECTORY "${_IF_EXIST_VAR}")
        list(APPEND ${_INNER_LIST_TO_ADD} "${_IF_EXIST_VAR}")
        message(STATUS "DIR EXISTS: ${_IF_EXIST_VAR}")
    else()
        message(STATUS "NO DIR EXISTS: ${_IF_EXIST_VAR}")
    endif()
endmacro()

macro(target_add_imported_bin_dir_if_exist_property_macro TGT _LIST_TO_ADD_VAR _PROP _FOUND)
    set(${_FOUND})
    get_property(_PROP_VAR TARGET ${TGT} PROPERTY ${_PROP})
    if(_PROP_VAR)
        get_filename_component(_PROP_VAR_DIR "${_PROP_VAR}" DIRECTORY REALPATH  )
        set(_PROP_VAR_DIR "${_PROP_VAR_DIR}/../bin")
        cmake_path(SET _PROP_VAR_DIR NORMALIZE "${_PROP_VAR_DIR}")
        message(STATUS "PROPERTY ${_PROP} OF TARGET ${TGT} IS FOUND. CHECKING FOR BIN DIR...")
        add_if_exist_macro(${_LIST_TO_ADD_VAR} "${_PROP_VAR_DIR}")
        set(${_FOUND} True)
    else()
        message(STATUS "NO BIN DIR OF PROPERTY ${_PROP} OF TARGET ${TGT} IS FOUND")
    endif()
endmacro()

macro(target_add_imported_bin_dir_if_exist TGT _LIST_TO_ADD)
    set(FOUND)
    foreach(PROP IN ITEMS IMPORTED_LOCATION_RELEASE IMPORTED_LOCATION_DEBUG IMPORTED_LOCATION)
        target_add_imported_bin_dir_if_exist_property_macro(${TGT} ${_LIST_TO_ADD} ${PROP} FOUND)
        if(FOUND)
            break()
        endif()
    endforeach()
endmacro()

macro(target_add_property_if_exist_macro TGT _LIST_TO_ADD_VAR _PROP _FOUND)
    set(${_FOUND})
    get_property(_PROP_VAR TARGET ${TGT} PROPERTY ${_PROP})
    if(_PROP_VAR)
        message(STATUS "PROPERTY ${_PROP} OF TARGET ${TGT} IS FOUND")
        list(APPEND ${_LIST_TO_ADD_VAR} "${_PROP_VAR}")
        set(${_FOUND} True)
    else()
        message(STATUS "NO PROPERTY ${_PROP} OF TARGET ${TGT} IS FOUND")
    endif()
endmacro()

macro(target_add_imported_property_if_exist TGT _LIST_TO_ADD)
    set(FOUND False)
    foreach(PROP IN ITEMS IMPORTED_LOCATION_RELEASE IMPORTED_LOCATION_DEBUG IMPORTED_LOCATION)
        target_add_property_if_exist_macro(${TGT} ${_LIST_TO_ADD} ${PROP} FOUND)
        if(FOUND)
            break()
        endif()
    endforeach()
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
            if(${TGT_TYPE} STREQUAL "SHARED_LIBRARY" OR ${TGT_TYPE} STREQUAL "UNKNOWN_LIBRARY")
                if(${TGT_TYPE} STREQUAL "SHARED_LIBRARY")
                   target_add_imported_property_if_exist(${TGT} ${_LIST_FILES_TO_ADD})
                endif(${TGT_TYPE} STREQUAL "SHARED_LIBRARY")
                target_add_imported_bin_dir_if_exist(${TGT} ${_LIST_DIRS_TO_ADD})
                get_property(_LINKED_LIBS TARGET ${TGT} PROPERTY INTERFACE_LINK_LIBRARIES)
                if(_LINKED_LIBS)
                    get_all_runtime_deps_and_dirs(${_LIST_FILES_TO_ADD} ${_LIST_DIRS_TO_ADD} ${_LINKED_LIBS})
                endif(_LINKED_LIBS)
            endif(${TGT_TYPE} STREQUAL "SHARED_LIBRARY" OR ${TGT_TYPE} STREQUAL "UNKNOWN_LIBRARY")
        endif(TARGET ${TGT})
    endforeach()
endmacro()


function(install_target_deps_recursive TGT)
    if(WIN32)
        get_all_runtime_deps_and_dirs(LIST_FILES_TO_ADD_VAR LIST_DIRS_TO_ADD_VAR ${ARGN})

        list(FILTER LIST_DIRS_TO_ADD_VAR EXCLUDE REGEX "[Cc]:[\\/][Ww][Ii][Nn][Dd][Oo][Ww][Ss][\\/][Ss][Yy][Ss][Tt][Ee][Mm]32.*")
        message(STATUS "Target's dep files: ${LIST_FILES_TO_ADD_VAR}")
        message(STATUS "Target's dep dirs: ${LIST_DIRS_TO_ADD_VAR}")

        #Transfer paths to install config
        install(CODE "set(LIST_FILES_TO_ADD_VAR_TRANSFER \"$<TARGET_FILE:${TGT}>;${LIST_FILES_TO_ADD_VAR}\")")
        install(CODE "set(LIST_DIRS_TO_ADD_VAR_TRANSFER \"${LIST_DIRS_TO_ADD_VAR}\")")
        install(CODE "set(CMAKE_INSTALL_FULL_BINDIR_TRANSFER \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\")")
        install(CODE "set(MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL_TRANSFER ${MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL})")

        # Also install shared library (DLL) dependencies
        install(CODE  [[
            file(GLOB _RUNTIME_FILES_TO_CHECK_LIST "${CMAKE_INSTALL_FULL_BINDIR_TRANSFER}/*.[ds][lo][l]")
            list(LENGTH _RUNTIME_FILES_TO_CHECK_LIST _RUNTIME_FILES_TO_CHECK_LIST_LENGTH)
            message(STATUS "_RUNTIME_FILES_TO_CHECK_LIST: ${_RUNTIME_FILES_TO_CHECK_LIST}")
            if(_RUNTIME_FILES_TO_CHECK_LIST_LENGTH GREATER_EQUAL ${MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL_TRANSFER})
                message(WARNING "There are enough runtime files found in the installation binary folder so no installation required, but you are able to set MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL_TRANSFER variable to change the skip threshold")
            else()
                file(GET_RUNTIME_DEPENDENCIES
                LIBRARIES
                    ${LIST_FILES_TO_ADD_VAR_TRANSFER}
                RESOLVED_DEPENDENCIES_VAR _r_deps
                UNRESOLVED_DEPENDENCIES_VAR _u_deps
                DIRECTORIES
                    ${LIST_DIRS_TO_ADD_VAR_TRANSFER}
                )
                foreach(_file ${_r_deps})
                    string(TOLOWER ${_file} _file_lower)
                    if(NOT ${_file_lower} MATCHES "c:[\\/]windows[\\/]system32.*")
                        file(INSTALL
                            DESTINATION "${CMAKE_INSTALL_FULL_BINDIR_TRANSFER}/"
                            TYPE SHARED_LIBRARY
                            FOLLOW_SYMLINK_CHAIN
                            FILES "${_file}"
                        )
                        list(APPEND _r_deps_not_system32 ${_file})
                    else()
                        get_filename_component(_FILE_NAME ${_file} NAME)
                        foreach(_dir ${LIST_DIRS_TO_ADD_VAR_TRANSFER})
                            if(EXISTS "${_dir}/${_FILE_NAME}")
                                file(INSTALL
                                DESTINATION "${CMAKE_INSTALL_FULL_BINDIR_TRANSFER}/"
                                TYPE SHARED_LIBRARY
                                FOLLOW_SYMLINK_CHAIN
                                FILES "${_dir}/${_FILE_NAME}")
                                list(APPEND _r_deps_not_system32 "${_dir}/${_FILE_NAME}")
                            endif()
                        endforeach()
                    endif()
                endforeach()
                message(STATUS "RESOLVED_DEPENDENCIES_VAR for target ${TGT}: ${_r_deps_not_system32}")
                message(STATUS "================================================================================================")
            endif(_RUNTIME_FILES_TO_CHECK_LIST_LENGTH GREATER_EQUAL ${MINIMUM_DEPENDENCIES_COUNT_TO_SKIP_INSTALL_TRANSFER})

        ]])
    endif()
endfunction()