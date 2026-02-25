# Integration tests for WinuxCmd
# These tests verify multi-command pipeline functionality

add_executable(pipeline_integration_test
    pipeline_integration_test.cpp
)

target_link_libraries(pipeline_integration_test
    PRIVATE
        winux-test
        test_main
)

target_include_directories(pipeline_integration_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/framework
)

target_precompile_headers(pipeline_integration_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/framework/framework_pch.h
)

# Set WINUXCMD_BIN_DIR for integration tests
file(REAL_PATH "${CMAKE_BINARY_DIR}" WINUXCMD_BIN_DIR_ABSOLUTE)
set(WINUXCMD_BIN_DIR "${WINUXCMD_BIN_DIR_ABSOLUTE}")
target_compile_definitions(pipeline_integration_test
    PRIVATE
    WINUXCMD_BIN_DIR=L"${WINUXCMD_BIN_DIR}"
)

# Add dependency on winuxcmd to ensure commands are built before tests run
add_dependencies(pipeline_integration_test winuxcmd)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(pipeline_integration_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTIES
        LABELS "integration;pipeline"
        TIMEOUT 60
)