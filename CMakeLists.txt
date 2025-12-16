cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(cascade)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use libc++ for C++ only
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++> -Wall -Wextra -Wpedantic)
add_link_options($<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)

# --- Sources ---
set(CASCADE_SOURCES
    src/main.cpp
    src/database.cpp
    src/PriorityQueue.cpp
    src/sorting.cpp
    src/repository.cpp
    src/util.cpp
)

# --- 1. Dependencies ---
find_package(SQLite3 REQUIRED)

# --- 2. Vendored Dependencies ---
# SQLiteCpp
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_DOXYGEN OFF CACHE BOOL "" FORCE)
set(SQLITECPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SQLITECPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SQLITECPP_INCLUDE_SCRIPT OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/sqlitecpp)

# FMT
add_subdirectory(lib/fmt)

# --- 3. Build Executable ---
add_executable(cascade ${CASCADE_SOURCES})

# DB Path definition
set(DATABASE_ABSOLUTE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/db/cascade.db")
target_compile_definitions(cascade PRIVATE "-DDATABASE_FILE=\"${DATABASE_ABSOLUTE_PATH}\"")

# --- 4. Include Paths (CRITICAL FIX) ---
# This tells the compiler where to find .h files
target_include_directories(cascade PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(cascade PRIVATE ${PROJECT_SOURCE_DIR}/lib/tabulate)
target_include_directories(cascade PRIVATE ${PROJECT_SOURCE_DIR}/lib/CLI11)

# --- 5. Linking ---
target_link_libraries(cascade PRIVATE 
    SQLiteCpp
    SQLite::SQLite3
    pthread 
    dl 
    c++abi
    fmt::fmt
)