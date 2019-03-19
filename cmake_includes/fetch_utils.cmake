# Arguments are assumed to be the names of dependencies that have been
# declared previously and should be populated. It is not an error if
# any of them have already been populated (they will just be skipped in
# that case). The command is implemented as a macro so that the variables
# defined by the FetchContent_GetProperties() and FetchContent_Populate()
# calls will be available to the caller.
macro(FetchContent_MakeAvailable)

    foreach(contentName IN ITEMS ${ARGV})
        string(TOLOWER ${contentName} contentNameLower)
        FetchContent_GetProperties(${contentName})
        if(NOT ${contentNameLower}_POPULATED)
            FetchContent_Populate(${contentName})

            # Only try to call add_subdirectory() if the populated content
            # can be treated that way. Protecting the call with the check
            # allows this function to be used for projects that just want
            # to ensure the content exists, such as to provide content at
            # a known location.
            if(EXISTS ${${contentNameLower}_SOURCE_DIR}/CMakeLists.txt)
                add_subdirectory(${${contentNameLower}_SOURCE_DIR}
                        ${${contentNameLower}_BINARY_DIR})
            endif()
        endif()
    endforeach()

endmacro()