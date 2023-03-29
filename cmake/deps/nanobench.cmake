if(TARGET nanobench::nanobench)
    return()
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE) # Show download progress

FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG f1743b497d57e946d8c476e9f89efb6d6c632b8a # 4.6.3
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(nanobench)

# Suppress warnings related to non-portable pragmas
target_compile_options(
    nanobench
    PRIVATE 
        -Wno-ignored-pragma-optimize
)

# Suppress std::getenv deprecation warning on Windows
if(WIN32)
    target_compile_definitions(
        nanobench
        PRIVATE 
            -D_CRT_SECURE_NO_WARNINGS
    )
endif()
