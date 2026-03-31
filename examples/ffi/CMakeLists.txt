cmake_minimum_required(VERSION 3.30.0)

# ====================================================
# FFI Example
# ====================================================
if(BUILD_FFI)
    # Original FFI example
    add_executable(ffi_example ffi_example.c)
    
    # Set include directory
    target_include_directories(ffi_example PRIVATE
        "${CMAKE_SOURCE_DIR}/src/ffi"
    )
    
    # Link FFI library target directly (NOT find_library)
    target_link_libraries(ffi_example PRIVATE winuxcmd_ffi)
    
    # MSVC specific
    if(MSVC)
        target_compile_options(ffi_example PRIVATE /std:c11)
    endif()
    
    message(STATUS "Added FFI example: ffi_example")
    
    # Copy DLL to output directory for Windows
    if(WIN32 AND MSVC)
        # For multi-config generators (VS), DLL is in $<CONFIG> subdirectory
        # For single-config generators (Ninja), DLL is directly in src/ffi/
        if(CMAKE_GENERATOR MATCHES "Ninja")
            add_custom_command(TARGET ffi_example POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                    "${CMAKE_BINARY_DIR}/src/ffi/winuxcmd.dll"
                    $<TARGET_FILE_DIR:ffi_example>
                COMMENT "Copying winuxcmd.dll to ffi_example directory"
            )
        else()
            add_custom_command(TARGET ffi_example POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                    "${CMAKE_BINARY_DIR}/src/ffi/$<CONFIG>/winuxcmd.dll"
                    $<TARGET_FILE_DIR:ffi_example>
                COMMENT "Copying winuxcmd.dll to ffi_example directory"
                )
        endif()
    endif()
endif()