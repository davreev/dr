add_executable(
    dr-bench
    main.cpp
    math_bench.cpp
    parallel_bench.cpp
)

include(deps/nanobench)

target_link_libraries(
    dr-bench
    PRIVATE
        dr::dr
        nanobench::nanobench
)

target_compile_options(
    dr-bench
    PRIVATE 
        -Wall -Wextra -Wpedantic -Werror
)

target_compile_definitions(
    dr-bench
    PRIVATE 
        # ...
)

target_link_options(
    dr-bench
    PRIVATE
        # ...
)
