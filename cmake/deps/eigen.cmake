if(TARGET Eigen3::Eigen)
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    eigen
    URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip
)

FetchContent_GetProperties(eigen)
if(NOT eigen_POPULATED)
    FetchContent_Populate(eigen)
endif()

add_library(Eigen3_Eigen INTERFACE)
add_library(Eigen3::Eigen ALIAS Eigen3_Eigen)

target_include_directories(
    Eigen3_Eigen
    SYSTEM # Suppresses warnings from third party headers
    INTERFACE
        "${eigen_SOURCE_DIR}"
)