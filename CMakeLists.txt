cmake_minimum_required(VERSION 3.30.0)
# CMake Module start from 3.30.0
# Ignore some warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)
if (POLICY CMP0158)
    cmake_policy(SET CMP0158 OLD)
endif()
if (POLICY CMP0174)
    cmake_policy(SET CMP0174 NEW)
endif()
string(REPLACE "/EHc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# =============
# Runtime Library Configuration
# Option to choose between static (MT) and dynamic (MD) CRT
option(USE_STATIC_CRT "Use statically linked CRT (MT) instead of dynamically linked (MD)" ON)
# Build mode selection: DEV for local development, RELEASE for production
# Handle legacy boolean values and invalid settings
if(DEFINED BUILD_MODE)
    # Convert boolean values to proper strings
    if(BUILD_MODE STREQUAL "ON" OR BUILD_MODE STREQUAL "OFF" OR BUILD_MODE STREQUAL "TRUE" OR BUILD_MODE STREQUAL "FALSE")
        message(STATUS "Converting legacy BUILD_MODE '${BUILD_MODE}' to 'DEV'")
        unset(BUILD_MODE CACHE)
        set(BUILD_MODE "DEV" CACHE STRING "Build mode (DEV or RELEASE)" FORCE)
    endif()
    
    # Validate build mode
    if(NOT BUILD_MODE STREQUAL "DEV" AND 
       NOT BUILD_MODE STREQUAL "RELEASE" AND 
       NOT BUILD_MODE STREQUAL "DEBUG_RELEASE")
        message(WARNING "Invalid BUILD_MODE '${BUILD_MODE}', resetting to 'DEV'")
        unset(BUILD_MODE CACHE)
        set(BUILD_MODE "DEV" CACHE STRING "Build mode (DEV or RELEASE)" FORCE)
    endif()
else()
    # Auto-detect build mode based on build type
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(BUILD_MODE "RELEASE" CACHE STRING "Build mode (DEV or RELEASE)" FORCE)
        message(STATUS "Auto-setting BUILD_MODE to RELEASE based on CMAKE_BUILD_TYPE=Release")
    else()
        set(BUILD_MODE "DEV" CACHE STRING "Build mode (DEV or RELEASE)" FORCE)
    endif()
endif()
set_property(CACHE BUILD_MODE PROPERTY STRINGS "DEV" "RELEASE" "DEBUG_RELEASE")

# Option to enable tests
option(ENABLE_TESTS "Enable tests" ON)

# Option to enable UPX compression
option(ENABLE_UPX "Enable UPX compression for executables" OFF)
# Generate the map info(For the developers to collect binary information)
option(GENERATE_MAP_INFO "Generate_MAP_INFO" ON)
# Merge Every commands.cpp into one source file(It could decrease template volume costs)
option(ENABLE_UNITY_BUILD "Enable Unity Build" ON)

# Configure options based on build mode
if(BUILD_MODE STREQUAL "DEV")
    # Development mode: optimize for fast incremental builds
    message(STATUS "Configuring for DEVELOPMENT mode")
    set(ENABLE_TESTS_DEFAULT ON)
    set(GENERATE_MAP_INFO_DEFAULT ON)
    set(ENABLE_UNITY_BUILD_DEFAULT OFF)  # Unity build slows down incremental compilation
    set(ENABLE_UPX_DEFAULT OFF)
elseif(BUILD_MODE STREQUAL "RELEASE")
    # Release mode: optimize for performance and size
    message(STATUS "Configuring for RELEASE mode")
    set(ENABLE_TESTS_DEFAULT OFF)
    set(GENERATE_MAP_INFO_DEFAULT OFF)
    set(ENABLE_UNITY_BUILD_DEFAULT ON)   # Unity build improves performance and reduces size
    set(ENABLE_UPX_DEFAULT OFF)
elseif(BUILD_MODE STREQUAL "DEBUG_RELEASE")
    # Debug-Release mode: Release optimizations with debug features
    message(STATUS "Configuring for DEBUG-RELEASE mode")
    set(ENABLE_TESTS_DEFAULT ON)
    set(GENERATE_MAP_INFO_DEFAULT ON)
    set(ENABLE_UNITY_BUILD_DEFAULT OFF)  # Disable Unity build for better debugging
    set(ENABLE_UPX_DEFAULT OFF)
else()
    message(WARNING "Unknown BUILD_MODE '${BUILD_MODE}', using DEVELOPMENT settings")
    set(ENABLE_TESTS_DEFAULT ON)
    set(GENERATE_MAP_INFO_DEFAULT ON)
    set(ENABLE_UNITY_BUILD_DEFAULT ON)
    set(ENABLE_UPX_DEFAULT OFF)
endif()

# Apply default values based on build mode if not explicitly set
if(NOT DEFINED ENABLE_TESTS_USER_VALUE)
    set(ENABLE_TESTS ${ENABLE_TESTS_DEFAULT} CACHE BOOL "Enable tests" FORCE)
endif()
if(NOT DEFINED GENERATE_MAP_INFO_USER_VALUE)
    set(GENERATE_MAP_INFO ${GENERATE_MAP_INFO_DEFAULT} CACHE BOOL "Generate_MAP_INFO" FORCE)
endif()
if(NOT DEFINED ENABLE_UNITY_BUILD_USER_VALUE)
    set(ENABLE_UNITY_BUILD ${ENABLE_UNITY_BUILD_DEFAULT} CACHE BOOL "Enable Unity Build" FORCE)
endif()
if(NOT DEFINED ENABLE_UPX_USER_VALUE)
    set(ENABLE_UPX ${ENABLE_UPX_DEFAULT} CACHE BOOL "Enable UPX compression for executables" FORCE)
endif()

if (USE_STATIC_CRT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
else ()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif ()

# Include UPX compression configuration if enabled
if (ENABLE_UPX)
    include(cmake/FetchUpx.cmake)
endif ()

if (NOT CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
endif ()

include(cmake/enable_import_std.cmake)
include(cmake/statistics_project.cmake)

project(WinuxCmd
        VERSION 0.4.5
        DESCRIPTION "Windows Compatible Linux Command Set"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/wd5050)    #we do not use rtti
    add_compile_options(/wd4819)    #we do not use unicode
endif ()

# Directory for compiled module files
set(MODULES_DIR "${CMAKE_CURRENT_BINARY_DIR}/modules")
file(MAKE_DIRECTORY "${MODULES_DIR}")

# Set source directory for header includes
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ===================== Unity Build Implementation =====================
if(ENABLE_UNITY_BUILD)
    function(enable_unity_build TARGET_NAME)
        get_target_property(TARGET_SOURCES ${TARGET_NAME} SOURCES)
        if(NOT TARGET_SOURCES)
            message(STATUS "Unity Build skipped for ${TARGET_NAME}: no sources found")
            return()
        endif()

        set(UNITY_SOURCE_FILES "")
        set(KEEP_SOURCE_FILES "")
        set(MODULE_FILES "")

        set(COMMANDS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/commands")

        foreach(SOURCE_FILE ${TARGET_SOURCES})
            if(NOT IS_ABSOLUTE ${SOURCE_FILE})
                set(ABS_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
            else()
                set(ABS_SOURCE_FILE ${SOURCE_FILE})
            endif()

            get_filename_component(SOURCE_EXT ${ABS_SOURCE_FILE} EXT)
            string(TOLOWER ${SOURCE_EXT} SOURCE_EXT_LOWER)

            if(SOURCE_EXT_LOWER MATCHES "\\.(cpp|cxx|cc)" AND ABS_SOURCE_FILE MATCHES "${COMMANDS_DIR}.*")
                if(EXISTS ${ABS_SOURCE_FILE})
                    list(APPEND UNITY_SOURCE_FILES ${ABS_SOURCE_FILE})
                endif()
            elseif(SOURCE_EXT_LOWER MATCHES "\\.(cppm)")
                list(APPEND MODULE_FILES ${SOURCE_FILE})
            else()
                list(APPEND KEEP_SOURCE_FILES ${SOURCE_FILE})
            endif()
        endforeach()

        list(LENGTH UNITY_SOURCE_FILES FILE_COUNT)
        if(FILE_COUNT LESS 2)
            message(STATUS "Unity Build skipped for ${TARGET_NAME}: too few command files (${FILE_COUNT})")
            return()
        endif()

        set(UNITY_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_commands_unity.cpp")
        file(WRITE ${UNITY_FILE} "// Auto-generated Unity Build file for ${TARGET_NAME} (only commands)\n")
        file(APPEND ${UNITY_FILE} "// Generated at: ${CMAKE_CURRENT_DATE}\n\n")

        foreach(SOURCE_FILE ${UNITY_SOURCE_FILES})
            string(REPLACE "\\" "\\\\" SOURCE_FILE_ESCAPED ${SOURCE_FILE})
            file(APPEND ${UNITY_FILE} "#include \"${SOURCE_FILE_ESCAPED}\"\n")
        endforeach()

        set_target_properties(${TARGET_NAME} PROPERTIES SOURCES "")

        if(KEEP_SOURCE_FILES)
            target_sources(${TARGET_NAME} PRIVATE ${KEEP_SOURCE_FILES})
        endif()

        if(MODULE_FILES)
            target_sources(${TARGET_NAME}
                    PUBLIC
                    FILE_SET CXX_MODULES
                    TYPE CXX_MODULES
                    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
                    FILES ${MODULE_FILES}
            )
        endif()
        target_sources(${TARGET_NAME} PRIVATE ${UNITY_FILE})

        target_compile_definitions(${TARGET_NAME} PRIVATE UNITY_BUILD=1)

        message(STATUS "Unity Build enabled for ${TARGET_NAME}: merged ${FILE_COUNT} command files into ${UNITY_FILE}")
    endfunction()
else()
    function(enable_unity_build TARGET_NAME)
        message(STATUS "Unity Build disabled for ${TARGET_NAME}")
    endfunction()
endif()


# Main executable: winuxcmd.exe - includes all commands
add_executable(winuxcmd
        src/Main/main.cpp
)

# PCH for winuxcmd
target_precompile_headers(winuxcmd PRIVATE ${CMAKE_SOURCE_DIR}/src/pch/pch.h)


# Add all C++23 module interfaces for main executable
file(GLOB_RECURSE CXX_MODULE_FILES CONFIGURE_DEPENDS
        "src/**/*.cppm"
)

target_sources(winuxcmd
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES
        ${CXX_MODULE_FILES}
)

file(GLOB COMMAND_SOURCES CONFIGURE_DEPENDS
        "src/commands/*.cpp"
)

target_sources(winuxcmd PRIVATE ${COMMAND_SOURCES})

# Default optimization flags
target_compile_options(winuxcmd PRIVATE
        $<$<CONFIG:Release>:/O2 /Os>
        /GR-  # Keep RTTI disabled for size
        /EHsc-
)

target_compile_definitions(winuxcmd PRIVATE
        _UNICODE
        UNICODE
        _CRT_DISABLE_PERFCRIT_LOCKS
        _HAS_RTTI=0
)

target_link_options(winuxcmd PRIVATE
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
)

if (USE_STATIC_CRT AND MSVC)
    target_compile_options(winuxcmd PRIVATE /MT /Os /Oy /GS-)
endif ()

if(GENERATE_MAP_INFO)
    target_link_options(winuxcmd PRIVATE
            /MAP:${CMAKE_CURRENT_BINARY_DIR}/winuxcmd.map
            /MAPINFO:EXPORTS
    )
endif ()

enable_unity_build(winuxcmd)

# Add UPX compression for main executable if enabled
if (ENABLE_UPX)
    add_upx_compression(winuxcmd)
endif ()

# Add include directories for main executable
target_include_directories(winuxcmd PRIVATE ${SRC_DIR})

# MSVC-specific configuration - common settings
if (MSVC)
    # Fix Windows SDK architecture errors

    # Enable linker optimizations
    set(CMAKE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} /LTCG")
endif ()

# Configuration for main executable
if (MSVC)
    target_compile_options(winuxcmd PRIVATE
            /std:c++latest        # Enable latest C++23 features including modules
            /GR-                  # Disable RTTI
    )

    target_compile_options(winuxcmd PRIVATE
            /experimental:module
            /interface
            "/translateInclude"
    )

    # Optimize for size in Release mode only
    target_compile_options(winuxcmd PRIVATE $<$<CONFIG:Release>:/O2 /Os>)

    # Module directory for compiled .ifc files
    target_include_directories(winuxcmd PRIVATE "${MODULES_DIR}")
endif ()

# Individual command executables - each only includes its own module

# Helper macro to create command executables
macro(create_command_executable CMD)
    add_executable(${CMD} src/Main/main.cpp)

    # Add only the necessary modules
    target_sources(${CMD}
            PUBLIC
            FILE_SET CXX_MODULES
            TYPE CXX_MODULES
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
            FILES
            src/core/dispatcher.cppm
            src/core/cmd_meta.cppm
            src/core/opt.cppm
            src/utils/utils.cppm
            src/utils/console.cppm
            src/utils/utf8.cppm
            src/Main/wildcard_handler.cppm
            src/container/container.cppm
            src/container/small_vector.cppm
            src/container/constexpr_map.cppm
    )

    target_sources(${CMD} PRIVATE src/commands/${CMD}.cpp)

    # MSVC-specific configuration for individual commands
    if (MSVC)
        target_compile_options(${CMD} PRIVATE
                /std:c++latest    # Enable latest C++23 features including modules
                /GR-              # Disable RTTI
                /experimental:module
                /interface
        )

        # Default optimization for size in Release mode
        target_compile_options(${CMD} PRIVATE
                $<$<CONFIG:Release>:/O2 /Os>
                /GR-  # Keep RTTI disabled for size
        )

        # Module directory for compiled .ifc files
        target_include_directories(${CMD} PRIVATE "${MODULES_DIR}")

        # Set source directory for header includes
        target_include_directories(${CMD} PRIVATE "${SRC_DIR}")

        # Add link options for release builds
        target_link_options(${CMD} PRIVATE $<$<CONFIG:Release>:
                /LTCG         # Link Time Code Generation
                /OPT:REF      # Eliminate unreferenced data
                /OPT:ICF      # Enable identical COMDAT folding
                >)

        # Add UPX compression for individual command executable if enabled
        if (ENABLE_UPX)
            add_upx_compression(${CMD})
        endif ()
    endif ()
endmacro()

# Automatically collect commands from src/commands directory
file(GLOB COMMAND_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/commands/*.cpp")
set(COMMANDS)
foreach (CMD_FILE ${COMMAND_FILES})
    # Extract command name from filename (without extension)
    get_filename_component(CMD_NAME "${CMD_FILE}" NAME_WE)
    list(APPEND COMMANDS ${CMD_NAME})
endforeach ()

# Note: Hardlinks are no longer created during build.
# Users should run create_links.ps1 in the bin directory to generate command links.

# Post-build command to create command links for testing
if (MSVC)
    add_custom_command(TARGET winuxcmd POST_BUILD
        COMMAND powershell.exe -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/scripts/create_links.ps1" -Force
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Creating command links for testing"
    )
endif ()

# Scaffold tool for generating new command modules
add_executable(scaffold
        src/tools/scaffold.cpp
)

# Set C++ standard for scaffold tool
set_target_properties(scaffold PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

# Add include directories for scaffold tool
target_include_directories(scaffold PRIVATE ${SRC_DIR})

# MSVC-specific configuration for scaffold tool
if (MSVC)
    target_compile_options(scaffold PRIVATE
            /std:c++latest
    )
endif ()

# Post-build command to copy scaffold to project root
add_custom_command(TARGET scaffold POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:scaffold>
        ${CMAKE_SOURCE_DIR}/scaffold.exe
        COMMENT "Copying scaffold to project root"
)

add_subdirectory(examples)

if (ENABLE_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif ()

include(GNUInstallDirs)

# Override default bin directory to install directly to root (no bin/ subdirectory)
# This maintains compatibility with older versions
set(CMAKE_INSTALL_BINDIR "." CACHE PATH "Output directory for executables" FORCE)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Installation prefix" FORCE)
endif ()

install(TARGETS winuxcmd
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/winux.ps1"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/winux-activate.ps1"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/winuxcmd_init.cmd"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/winuxcmd_uninstall.cmd"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_links.ps1"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)

install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/README-ZH.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        DESTINATION "."
        COMPONENT Documentation
)

set(CPACK_PACKAGE_NAME "WinuxCmd")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Windows Compatible Linux Command Set")
set(CPACK_PACKAGE_VENDOR "WinuxCmd Project")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "WinuxCmd")

set(CPACK_COMPONENTS_ALL Runtime Documentation)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_RUNTIME_GROUP "Core")
set(CPACK_COMPONENT_DOCUMENTATION_GROUP "Documentation")
set(CPACK_COMPONENT_GROUP_CORE_DISPLAY_NAME "Core Components")
set(CPACK_COMPONENT_GROUP_DOCUMENTATION_DISPLAY_NAME "Documentation")

if (WIN32)
    set(CPACK_GENERATOR "ZIP;TGZ;7Z")
    set(CPACK_ARCHIVE_SEVEN_ZIP_OPTIONS "-mx=9")
    set(CPACK_ARCHIVE_TAR_GZIP_OPTIONS "-9")
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win-arm64")
    else ()
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win-x64")
    endif ()
endif ()

set(CPACK_IGNORE_FILES
        "${CMAKE_SOURCE_DIR}/scaffold.exe$"
        "${CMAKE_SOURCE_DIR}/scaffold"
        "${CMAKE_SOURCE_DIR}/CMakeLists.txt$"
        "${CMAKE_SOURCE_DIR}/.clang.*$"
        "${CMAKE_SOURCE_DIR}/.git.*$"
        "${CMAKE_SOURCE_DIR}/.vs/"
        "${CMAKE_SOURCE_DIR}/.idea/"
        "${CMAKE_SOURCE_DIR}/src/"
        "${CMAKE_SOURCE_DIR}/test/"
        "${CMAKE_SOURCE_DIR}/cmake-build-.*/"
        "${CMAKE_SOURCE_DIR}/out/"
        "${CMAKE_SOURCE_DIR}/TemplateLicense.lict$"
        "${CMAKE_SOURCE_DIR}/tests"
)

# Print final configuration summary
message(STATUS "=================== BUILD CONFIGURATION SUMMARY ===================")
message(STATUS "Build Mode: ${BUILD_MODE}")
message(STATUS "Tests Enabled: ${ENABLE_TESTS}")
message(STATUS "Map Info Generation: ${GENERATE_MAP_INFO}")
message(STATUS "Unity Build: ${ENABLE_UNITY_BUILD}")
message(STATUS "UPX Compression: ${ENABLE_UPX} (PERMANENTLY DISABLED)")
message(STATUS "Static CRT: ${USE_STATIC_CRT}")
message(STATUS "Note: UPX compression has been permanently disabled for reliability")
message(STATUS "Note: Run create_links.ps1 in bin directory to generate command links")
message(STATUS "=================================================================")

# Print out supported the amount of supported commands
statistics_number_of_commands_supported()
message(STATUS "Final count result: ${COMMANDS_SRC_FILE_COUNT} .cpp files")

# ========== Benchmark (DEV and DEBUG_RELEASE mode only) ==========
if (BUILD_MODE STREQUAL "DEV" OR BUILD_MODE STREQUAL "DEBUG_RELEASE")
    message(STATUS "Benchmarks: ENABLED (${BUILD_MODE} mode)")
    add_subdirectory(benchmark)
else ()
    message(STATUS "Benchmarks: DISABLED (only available in DEV and DEBUG_RELEASE mode)")
endif ()

include(CPack)
