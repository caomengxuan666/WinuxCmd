cmake_minimum_required(VERSION 3.30.0)

# ====================================================
# FFI Example
# ====================================================
if(BUILD_FFI)
    add_executable(ffi_example ffi_example.c)
    
    # Find FFI library
    find_library(FFI_LIBRARY NAMES winuxcmd 
        PATHS "${CMAKE_BINARY_DIR}/src/ffi"
        NO_DEFAULT_PATH)
    
    if(FFI_LIBRARY)
        target_link_libraries(ffi_example PRIVATE ${FFI_LIBRARY})
        
        # Set include directory
        target_include_directories(ffi_example PRIVATE
            "${CMAKE_SOURCE_DIR}/src/ffi"
        )
        
        # MSVC specific
        if(MSVC)
            target_compile_options(ffi_example PRIVATE /std:c11)
        endif()
        
        message(STATUS "Added FFI example: ffi_example")
        
        # Copy DLL to output directory for Windows
        if(WIN32 AND MSVC)
            add_custom_command(TARGET ffi_example POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CMAKE_BINARY_DIR}/src/ffi/$<CONFIG>/winuxcmd.dll"
                    $<TARGET_FILE_DIR:ffi_example>
                COMMENT "Copying winuxcmd.dll to ffi_example directory"
            )
        endif()
    else()
        message(WARNING "FFI library not found, skipping ffi_example")
        set_target_properties(ffi_example PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
endif()