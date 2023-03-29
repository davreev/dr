if(TARGET utest::utest)
    return()
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE) # Show download progress

FetchContent_Declare(
    utest
    GIT_REPOSITORY https://github.com/sheredom/utest.h.git
    GIT_TAG 7582e54960503b0279e49f2c3bbce1d0a925d361
    GIT_PROGRESS TRUE
)

FetchContent_GetProperties(utest)
if(NOT ${utest_POPULATED})
    FetchContent_Populate(utest)
endif()

add_library(utest INTERFACE)
add_library(utest::utest ALIAS utest)

target_include_directories(
    utest 
    SYSTEM # Suppresses warnings from third party headers
    INTERFACE 
        "${utest_SOURCE_DIR}"
)

# Target includes windows.h without defines for avoiding macro clash :(
if(WIN32)
    target_compile_definitions(
        utest
        INTERFACE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )
endif()
