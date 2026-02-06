cmake_minimum_required(VERSION 3.30)
project(winuxcmd-tests)

enable_testing()

# =========================
# framework
# =========================
file(GLOB WINUX_TEST_FRAMEWORK_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/framework/*.cpp
)

add_library(winux-test STATIC
        ${WINUX_TEST_FRAMEWORK_SOURCES}
)

target_include_directories(winux-test
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

set(WINUXCMD_BIN_DIR ${CMAKE_BINARY_DIR})
# use binary dir to locate the executable like ls ,cat...
target_compile_definitions(winux-test
        PUBLIC
        WINUXCMD_BIN_DIR=L"${WINUXCMD_BIN_DIR}"
)
# =========================
# tests
# =========================
file(GLOB_RECURSE WINUXCMD_TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp
)

foreach (src ${WINUXCMD_TEST_SOURCES})
    if (src MATCHES "/framework/")
        continue()
    endif ()

    get_filename_component(test_name ${src} NAME_WE)
    get_filename_component(test_dir ${src} DIRECTORY)
    get_filename_component(group ${test_dir} NAME)

    set(target winuxcmd-test-${group}-${test_name})

    add_executable(${target} ${src})

    target_link_libraries(${target} PRIVATE winux-test)

    target_include_directories(${target}
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_precompile_headers(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/framework/winuxtest.h)

    add_test(
            NAME ${group}.${test_name}
            COMMAND ${target}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endforeach ()
