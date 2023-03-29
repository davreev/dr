if(TARGET OpenMP::OpenMP_CXX)
    return()
endif()

find_package(OpenMP)

if(TARGET OpenMP::OpenMP_CXX)
    # https://gitlab.kitware.com/cmake/cmake/-/issues/21818
    target_link_options(
        OpenMP::OpenMP_CXX
        INTERFACE
            -fopenmp
    )
endif()
