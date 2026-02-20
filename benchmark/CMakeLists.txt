# Benchmark Configuration (DEV mode only)

include(FetchContent)

# Download Google Benchmark library
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.0
    GIT_SHALLOW TRUE
)

# Set options BEFORE calling MakeAvailable
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)

# Try to make available
if (NOT googlebenchmark_POPULATED)
    FetchContent_MakeAvailable(googlebenchmark)
endif ()

if (NOT googlebenchmark_POPULATED)
    message(WARNING "Failed to download Google Benchmark library. Benchmarks will be unavailable.")
    return()
endif ()

# Create benchmark executable
add_executable(winuxcmd_benchmarks
    benchmark_main.cpp
)

# Link to benchmark library
target_link_libraries(winuxcmd_benchmarks PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
)

# Include source directory for benchmark tests
target_include_directories(winuxcmd_benchmarks PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# MSVC specific options (same as examples)
if(MSVC)
    target_compile_options(winuxcmd_benchmarks PRIVATE
        /std:c++latest
        /experimental:module
    )
    target_include_directories(winuxcmd_benchmarks PRIVATE
        "${CMAKE_BINARY_DIR}/modules"
    )
    target_link_libraries(winuxcmd_benchmarks PRIVATE
        container_module
    )
endif()

# Add benchmark subdirectories
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmd_benchmarks")
    add_subdirectory(cmd_benchmarks)
endif ()