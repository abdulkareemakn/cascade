cmake_minimum_required(VERSION 3.10)

project(argon2 C)

set(ARGON2_SRC
    src/argon2.c
    src/core.c
    src/blake2/blake2b.c
    src/thread.c
    src/encoding.c
    src/ref.c
)

add_library(argon2 STATIC ${ARGON2_SRC})

target_include_directories(argon2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(argon2 PRIVATE
    -std=c89
    -O3
    -Wall
)

find_package(Threads REQUIRED)
target_link_libraries(argon2 PRIVATE Threads::Threads)
