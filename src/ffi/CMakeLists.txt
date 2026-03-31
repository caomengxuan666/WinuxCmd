# FFI Library - Dynamic library for external integration
add_library(winuxcmd_ffi SHARED
    ffi.cpp
)

# Add PCH for FFI library
target_precompile_headers(winuxcmd_ffi PRIVATE ${CMAKE_SOURCE_DIR}/src/pch/pch.h)

# Define module paths relative to project root
set(PROJECT_MODULE_BASE "${CMAKE_CURRENT_SOURCE_DIR}/..")

# Define all required modules and partitions
set(FFI_CORE_MODULES
    ${PROJECT_MODULE_BASE}/core/core.cppm
    ${PROJECT_MODULE_BASE}/core/cmd_meta.cppm
    ${PROJECT_MODULE_BASE}/core/command_context.cppm
    ${PROJECT_MODULE_BASE}/core/dispatcher.cppm
    ${PROJECT_MODULE_BASE}/core/opt.cppm
    ${PROJECT_MODULE_BASE}/core/pipeline.cppm
)

set(FFI_CONTAINER_MODULES
    ${PROJECT_MODULE_BASE}/container/container.cppm
    ${PROJECT_MODULE_BASE}/container/small_vector.cppm
    ${PROJECT_MODULE_BASE}/container/constexpr_map.cppm
)

set(FFI_UTILS_MODULES
    ${PROJECT_MODULE_BASE}/utils/utils.cppm
    ${PROJECT_MODULE_BASE}/utils/utf8.cppm
    ${PROJECT_MODULE_BASE}/utils/console.cppm
    ${PROJECT_MODULE_BASE}/utils/text_io.cppm
    ${PROJECT_MODULE_BASE}/utils/path.cppm
    ${PROJECT_MODULE_BASE}/utils/wildcard.cppm
    ${PROJECT_MODULE_BASE}/utils/json.cppm
    ${PROJECT_MODULE_BASE}/utils/encoding.cppm
    ${PROJECT_MODULE_BASE}/utils/file_io.cppm
    ${PROJECT_MODULE_BASE}/utils/cppbar.cppm
)

set(FFI_VERSION_MODULES
    ${PROJECT_MODULE_BASE}/version/version.cppm
)

# Combine all modules
set(FFI_ALL_MODULES
    ${FFI_CORE_MODULES}
    ${FFI_CONTAINER_MODULES}
    ${FFI_UTILS_MODULES}
    ${FFI_VERSION_MODULES}
)

# Add all necessary modules for FFI library
target_sources(winuxcmd_ffi
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    BASE_DIRS "${PROJECT_MODULE_BASE}"
    FILES ${FFI_ALL_MODULES}
)

# Set output name to winuxcore.dll
set_target_properties(winuxcmd_ffi PROPERTIES
    OUTPUT_NAME "winuxcore"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Export DLL symbols
target_compile_definitions(winuxcmd_ffi PRIVATE
    WINUX_FFI_EXPORTS
)

# Set C++ standard and options
set_target_properties(winuxcmd_ffi PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Compile options
target_compile_options(winuxcmd_ffi PRIVATE
    $<$<CONFIG:Release>:/O2 /Os>
    /GR-  # Disable RTTI
)

target_compile_definitions(winuxcmd_ffi PRIVATE
    _UNICODE
    UNICODE
    _CRT_DISABLE_PERFCRIT_LOCKS
    _HAS_RTTI=0
)

# Link options
target_link_options(winuxcmd_ffi PRIVATE
    $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
)

if (USE_STATIC_CRT AND MSVC)
    target_compile_options(winuxcmd_ffi PRIVATE /MT /Os /Oy /GS-)
endif ()

# MSVC-specific configuration
if (MSVC)
    target_compile_options(winuxcmd_ffi PRIVATE
        /std:c++latest
        /experimental:module
        /interface
    )
    
    # Module directory for compiled .ifc files
    target_include_directories(winuxcmd_ffi PRIVATE "${MODULES_DIR}")
endif ()

# Add include directories
target_include_directories(winuxcmd_ffi PRIVATE ${SRC_DIR} ${CMAKE_BINARY_DIR})

# Link Windows libraries
target_link_libraries(winuxcmd_ffi PRIVATE
    kernel32.lib
    user32.lib
    advapi32.lib
)

# Install FFI library and files to ffi directory
install(TARGETS winuxcmd_ffi
    LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}/ffi"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/ffi"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_BINDIR}/ffi"
)

# Install header file
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/ffi.h"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/ffi"
)