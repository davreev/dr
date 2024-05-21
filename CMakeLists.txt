cmake_minimum_required(VERSION 3.14)
project(
    dr
    VERSION 0.1.0
    LANGUAGES C CXX
)

#
# Global config
#

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

list(
    PREPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/cmake/"
)

# Show FetchContent download progress
set(FETCHCONTENT_QUIET FALSE)

#
# Library target
#

add_library(
    dr STATIC
    "src/halfedge.cpp"
    "src/memory.cpp"
    "src/mesh_primitives.cpp"
)
add_library(dr::dr ALIAS dr)

target_include_directories(
    dr
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

include(deps/eigen)
include(deps/fmt)
include(deps/openmp)
include(deps/unordered-dense)

target_link_libraries(
    dr
    PUBLIC
        Eigen3::Eigen
        fmt::fmt
        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>
        unordered_dense::unordered_dense
)

target_compile_options(
    dr
    PRIVATE
        -Wall -Wextra -Wpedantic -Werror
)

target_compile_definitions(
    dr
    PUBLIC
        EIGEN_INITIALIZE_MATRICES_BY_ZERO
)

target_link_options(
    dr
    PRIVATE
        # ...
)

#
# Optional targets
#

option(DR_TEST "Generate test target" OFF)
if(DR_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
