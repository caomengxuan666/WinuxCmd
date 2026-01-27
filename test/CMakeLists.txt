# Test directory CMakeLists.txt

# Add test executable
add_executable(test_commands test_commands.cpp)

# Link Catch2 library
target_link_libraries(test_commands PRIVATE Catch2::Catch2WithMain Catch2::Catch2)

# Set C++ standard
target_compile_features(test_commands PRIVATE cxx_std_23)

# Add include directories
target_include_directories(test_commands PRIVATE ${CMAKE_SOURCE_DIR}/src)

# MSVC-specific configuration
if(MSVC)
    target_compile_options(test_commands PRIVATE
            /std:c++latest        # Enable latest C++23 features including modules
            /GR-
    )
endif()

# Add test to CTest
include(CTest)
include(Catch)
catch_discover_tests(test_commands)