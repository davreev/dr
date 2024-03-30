if(TARGET utest::utest)
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    utest
    URL https://raw.githubusercontent.com/sheredom/utest.h/7582e54960503b0279e49f2c3bbce1d0a925d361/utest.h
    DOWNLOAD_NO_EXTRACT TRUE
)

FetchContent_GetProperties(utest)
if(NOT ${utest_POPULATED})
    FetchContent_Populate(utest)
endif()

# Copy header to the expected include dir
file(
    COPY 
        "${utest_SOURCE_DIR}/utest.h" 
    DESTINATION 
        "${utest_SOURCE_DIR}/include/utest"
)

add_library(utest INTERFACE)
add_library(utest::utest ALIAS utest)

target_include_directories(
    utest 
    SYSTEM # Suppresses warnings from third party headers
    INTERFACE 
        "${utest_SOURCE_DIR}/include"
)

# Avoid windows.h min/max macro clash
if(WIN32)
    target_compile_definitions(
        utest
        INTERFACE
            NOMINMAX
    )
endif()
