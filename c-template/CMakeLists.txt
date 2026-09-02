cmake_minimum_required(VERSION 3.16)

project(labuladong_c_template VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(LC_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/leetcode/editor/common")

add_library(lc_common STATIC
    "${LC_COMMON_DIR}/lc_nodes.c"
    "${LC_COMMON_DIR}/cstl/vos_common.c"
    "${LC_COMMON_DIR}/cstl/vos_vector.c"
    "${LC_COMMON_DIR}/cstl/vos_priorityqueue.c"
)

target_include_directories(lc_common PUBLIC
    "${LC_COMMON_DIR}"
    "${LC_COMMON_DIR}/cstl"
    "${LC_COMMON_DIR}/third_party/uthash"
)

if(MSVC)
    target_compile_options(lc_common PRIVATE /W4)
else()
    target_compile_options(lc_common PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Each generated problem contains its own main(), so every file is a separate
# executable. EXCLUDE_FROM_ALL keeps unfinished solutions from breaking tests.
file(GLOB_RECURSE LC_PROBLEM_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/leetcode/editor/cn/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/leetcode/editor/en/*.c"
)

foreach(source_file IN LISTS LC_PROBLEM_SOURCES)
    file(RELATIVE_PATH relative_file
        "${CMAKE_CURRENT_SOURCE_DIR}/leetcode"
        "${source_file}"
    )
    string(REPLACE "\\" "/" relative_file "${relative_file}")
    string(REGEX REPLACE "\\.c$" "" target_name "${relative_file}")
    string(REGEX REPLACE "[^A-Za-z0-9_]" "_" target_name "${target_name}")
    set(target_name "lc_${target_name}")

    add_executable("${target_name}" EXCLUDE_FROM_ALL "${source_file}")
    target_link_libraries("${target_name}" PRIVATE lc_common)
endforeach()

include(CTest)
if(BUILD_TESTING)
    add_executable(c_template_tests "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_all.c")
    target_link_libraries(c_template_tests PRIVATE lc_common)
    if(MSVC)
        target_compile_options(c_template_tests PRIVATE /W4)
    else()
        target_compile_options(c_template_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME c_template_tests COMMAND c_template_tests)
endif()
