cmake_minimum_required(VERSION 3.30.0)
# CMake Module start from 3.30.0

# Runtime Library Configuration
# Option to choose between static (MT) and dynamic (MD) CRT
option(USE_STATIC_CRT "Use statically linked CRT (MT) instead of dynamically linked (MD)" ON)
# Option to enable tests
option(ENABLE_TESTS "Enable tests" OFF)
# Allow users to debug by the .i file.
option(GENERATE_EXPAND_MACRO_FILE "Generate sourcefile.i file to show the macro" OFF)
# Option to use hardlinks for commands instead of separate executables
option(USE_HARDLINKS "Use hardlinks for commands instead of separate executables" ON)
# Option to enable UPX compression
option(ENABLE_UPX "Enable UPX compression for executables" OFF)

if (USE_STATIC_CRT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
else ()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif ()

if (GENERATE_EXPAND_MACRO_FILE)
    if (MSVC)
        target_compile_options(ls PRIVATE /P)
    endif ()
endif ()

# Include UPX compression configuration if enabled
if (ENABLE_UPX)
    include(cmake/FetchUpx.cmake)
endif()

if (NOT CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
endif ()

include(cmake/enable_import_std.cmake)

project(WinuxCmd
        VERSION 0.1.0
        DESCRIPTION "Windows Compatible Linux Command Set"
        LANGUAGES CXX
)


if (MSVC)
    # remove default Ehsc
    string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    # use Ehsc- instead to disable Exception handlers.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc-")
    add_compile_options(/wd9025)
    add_compile_definitions(NOMINMAX)
endif ()

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

# Main executable: winuxcmd.exe - includes all commands
add_executable(winuxcmd
        src/main.cpp
)

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

# Default optimization flags
target_compile_options(winuxcmd PRIVATE
        $<$<CONFIG:Release>:/O2 /Os>
        /GR-  # Keep RTTI disabled for size
)

target_link_options(winuxcmd PRIVATE
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
)

# Add UPX compression for main executable if enabled
if (ENABLE_UPX)
    add_upx_compression(winuxcmd)
endif()

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
    add_executable(${CMD} src/main.cpp)

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
            src/commands/${CMD}.cppm
    )

    # Add only the necessary implementation file
    #target_sources(${CMD} PRIVATE src/core/dispatcher.cpp)

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
        endif()
    endif ()
endmacro()

# Automatically collect commands from src/commands directory
file(GLOB COMMAND_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/commands/*.cppm")
set(COMMANDS)
foreach(CMD_FILE ${COMMAND_FILES})
    # Extract command name from filename (without extension)
    get_filename_component(CMD_NAME "${CMD_FILE}" NAME_WE)
    list(APPEND COMMANDS ${CMD_NAME})
endforeach()

# Create individual command executables or hardlinks
if (NOT USE_HARDLINKS)
    foreach(CMD ${COMMANDS})
        create_command_executable(${CMD})
    endforeach()
else()
    # Create hardlinks after winuxcmd is built
    foreach(CMD ${COMMANDS})
        add_custom_command(TARGET winuxcmd POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_hardlink
            $<TARGET_FILE:winuxcmd>
            "$<TARGET_FILE_DIR:winuxcmd>/${CMD}.exe"
            COMMENT "Creating hardlink for ${CMD}"
        )
    endforeach()
endif()

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

if (ENABLE_TESTS)
    # Add Catch2 test framework
    include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(Catch2)

    # Add test directory
    add_subdirectory(test)
endif ()

# Install configuration
include(GNUInstallDirs)

# Set installation prefix
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Installation prefix" FORCE)
endif ()

# Install executables (exclude scaffold)
if (NOT USE_HARDLINKS)
    install(TARGETS winuxcmd ls cat cp mv rm mkdir
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT Runtime
    )
else()
    # Install winuxcmd and create hardlinks in install directory
    install(TARGETS winuxcmd
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT Runtime
    )
    
    # Create hardlinks for commands in install directory
    foreach(CMD ${COMMANDS})
        install(CODE "
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E create_hardlink
                ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/winuxcmd.exe
                ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${CMD}.exe
                RESULT_VARIABLE result
                ERROR_QUIET
            )
            # If hardlink creation fails (e.g., on some filesystems), create a copy instead
            if(result)
                execute_process(
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/winuxcmd.exe
                    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${CMD}.exe
                )
            endif()
        " COMPONENT Runtime)
    endforeach()
endif()

# Install activation script
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/winux-activate.ps1"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)

# Install documentation
install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/README-ZH.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        COMPONENT Documentation
)

# CPack configuration
set(CPACK_PACKAGE_NAME "WinuxCmd")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Windows Compatible Linux Command Set")
set(CPACK_PACKAGE_VENDOR "WinuxCmd Project")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "WinuxCmd")

# Set package components
set(CPACK_COMPONENTS_ALL Runtime Documentation)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")

# Set component groups
set(CPACK_COMPONENT_RUNTIME_GROUP "Core")
set(CPACK_COMPONENT_DOCUMENTATION_GROUP "Documentation")

set(CPACK_COMPONENT_GROUP_CORE_DISPLAY_NAME "Core Components")
set(CPACK_COMPONENT_GROUP_DOCUMENTATION_DISPLAY_NAME "Documentation")

# Windows-specific settings
if (WIN32)
    set(CPACK_GENERATOR "ZIP")
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win-arm64")
    else()
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win-x64")
    endif()
else ()
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-linux")
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
)

# Include CPack
include(CPack)