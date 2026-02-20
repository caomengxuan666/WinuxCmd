cmake_minimum_required(VERSION 3.30)
project(winuxcmd-tests)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB WINUX_TEST_FRAMEWORK_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/framework/*.cpp"
)

add_library(test_main STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/framework/test_main.cpp
)

target_precompile_headers(test_main PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/framework/framework_pch.h
)

list(FILTER WINUX_TEST_FRAMEWORK_SOURCES EXCLUDE REGEX ".*test_main\\.cpp$")

add_library(winux-test STATIC
        ${WINUX_TEST_FRAMEWORK_SOURCES}
)

target_precompile_headers(winux-test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/framework/framework_pch.h
)

target_include_directories(winux-test
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(test_main
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# Use absolute path for WINUXCMD_BIN_DIR to ensure tests work correctly
# regardless of the current working directory
file(REAL_PATH "${CMAKE_BINARY_DIR}" WINUXCMD_BIN_DIR_ABSOLUTE)
set(WINUXCMD_BIN_DIR "${WINUXCMD_BIN_DIR_ABSOLUTE}")
target_compile_definitions(winux-test
        PUBLIC
        WINUXCMD_BIN_DIR=L"${WINUXCMD_BIN_DIR}"
)

target_compile_definitions(test_main
        PUBLIC
        WINUXCMD_BIN_DIR=L"${WINUXCMD_BIN_DIR}"
)

file(GLOB_RECURSE ALL_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/unit/**/*.cpp"
)

list(FILTER ALL_TEST_SOURCES EXCLUDE REGEX ".*/framework/.*")

message(STATUS "=== WinuxCmd Test Configuration ===")
message(STATUS "Found test sources: ${ALL_TEST_SOURCES}")

if(NOT ALL_TEST_SOURCES)
    message(WARNING "No test sources found in tests/unit/ directory!")
else()
    add_executable(winuxcmd-tests
            ${ALL_TEST_SOURCES}
    )

    target_link_libraries(winuxcmd-tests PRIVATE winux-test test_main)
    
    target_include_directories(winuxcmd-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_precompile_headers(winuxcmd-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/framework/framework_pch.h)

    include(GoogleTest)
    
    # Only discover individual tests - TestMate will create hierarchy from these
    gtest_discover_tests(winuxcmd-tests
        EXTRA_ARGS ""
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        TEST_PREFIX ""
        PROPERTIES LABELS "WinuxCmd"
    )
    
    # Custom targets for convenience (not CTest entries)
    add_custom_target(test-all
        COMMAND $<TARGET_FILE:winuxcmd-tests>
        DEPENDS winuxcmd-tests
        COMMENT "Running all WinuxCmd unit tests..."
    )
    
    add_custom_target(list-tests
        COMMAND $<TARGET_FILE:winuxcmd-tests> --list-tests
        DEPENDS winuxcmd-tests
        COMMENT "Listing all test cases..."
    )
    
    # Auto-detect groups for custom targets only
    set(TEST_GROUPS "")
    foreach(src ${ALL_TEST_SOURCES})
        get_filename_component(test_dir ${src} DIRECTORY)
        file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR}/unit ${test_dir})
        get_filename_component(group_name ${rel_path} NAME)
        
        if(NOT "${group_name}" STREQUAL "" AND NOT "${group_name}" IN_LIST TEST_GROUPS)
            list(APPEND TEST_GROUPS ${group_name})
            add_custom_target(test-${group_name}
                COMMAND $<TARGET_FILE:winuxcmd-tests> --gtest_filter=${group_name}.*
                DEPENDS winuxcmd-tests
                COMMENT "Running ${group_name} tests..."
            )
        endif()
    endforeach()
endif()

option(EXCLUDE_TESTS_FROM_DEFAULT_BUILD "Exclude test targets from default build" OFF)
if(EXCLUDE_TESTS_FROM_DEFAULT_BUILD)
    set_property(TARGET winuxcmd-tests PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    add_custom_target(build-tests
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target winuxcmd-tests
            COMMENT "Building test executable..."
    )
endif()