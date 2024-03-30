if(TARGET unordered_dense::unordered_dense)
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    unordered-dense
    URL https://github.com/martinus/unordered_dense/archive/refs/tags/v4.4.0.zip
)

FetchContent_MakeAvailable(unordered-dense)
