add_executable(
    dr-test
    allocator_tests.cpp
    bitwise_tests.cpp
    container_tests.cpp
    defer_tests.cpp
    diagnostics_tests.cpp
    function_ref_tests.cpp
    geometry_tests.cpp
    grid_tests.cpp
    halfedge_tests.cpp
    hash_grid_tests.cpp
    linalg_tests.cpp
    main.cpp
    math_tests.cpp
    memory_tests.cpp
    mesh_attributes_tests.cpp
    mesh_incidence_tests.cpp
    mesh_repair_tests.cpp
    mesh_utils_tests.cpp
    meta_tests.cpp
    random_tests.cpp
    sliced_array_tests.cpp
    slot_map_tests.cpp
    span_tests.cpp
    transform_tests.cpp
    triangulate_tests.cpp
)

include(deps/utest)

target_link_libraries(
    dr-test 
    PRIVATE
        dr::dr
        utest::utest
)

target_compile_options(
    dr-test
    PRIVATE 
        -Wall -Wextra -Wpedantic -Werror
)

# Allow std::fopen on Windows builds
if(WIN32)
    target_compile_options(
        dr-test
        PRIVATE 
            -Wno-deprecated-declarations
    )
endif()

add_test(
    NAME dr-test
    COMMAND dr-test
)
