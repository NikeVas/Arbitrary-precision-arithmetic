project(BIGINTEGER)
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

set(ASAN_COMPILE_FLAGS -fsanitize=address,undefined -fno-sanitize-recover=all)
set(ASAN_LINK_FLAGS -fsanitize=address,undefined)

add_compile_options(${ASAN_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ASAN_LINK_FLAGS}")

add_compile_options(-Wall -Wextra -pedantic)

add_library(biginteger biginteger.cpp)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC biginteger)


# --------------------

function (make_biginteger_test name)
    add_executable("${name}" ${ARGN})
    add_test(NAME "${name}" COMMAND "${name}")
    target_link_libraries("${name}" PUBLIC biginteger)
    target_include_directories("${name}" PUBLIC ${CMAKE_SOURCE_DIR})
endfunction()

# --------------------

enable_testing()
include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(test)