# MSVC Toolchain for WinuxCmd
# Ensures MSVC compiler is used for all builds

set(CMAKE_C_COMPILER cl CACHE FILEPATH "C Compiler")
set(CMAKE_CXX_COMPILER cl CACHE FILEPATH "C++ Compiler")

# Use static runtime library
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")

message(STATUS "Using MSVC compiler toolchain")