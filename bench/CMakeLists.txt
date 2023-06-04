function(make_bench_target TARGET_NAME TARGET_SRC)

    add_executable(
        ${TARGET_NAME}
        ${TARGET_SRC}
    )

    target_link_libraries(
        ${TARGET_NAME}
        PRIVATE
            dr::dr
            nanobench::nanobench
    )
    
    target_compile_options(
        ${TARGET_NAME}
        PRIVATE 
            -Wall -Wextra -Wpedantic -Werror
    )

endfunction()

include(deps/nanobench)

make_bench_target(mat3-multiply mat3_multiply.cpp)
make_bench_target(mat4-solve mat4_solve.cpp)
make_bench_target(mod mod.cpp)
make_bench_target(parallel-for parallel_for.cpp)
