cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(cascade)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global options for the project
add_compile_options(-stdlib=libc++ -Wall -Wextra -Wpedantic)
add_link_options(-stdlib=libc++)

# --- Define Project Sources Explicitly (Recommended) ---
# Add all your .cpp files here. This is safer than using GLOB.
set(CASCADE_SOURCES
    src/main.cpp
    src/database.cpp
    src/security.cpp
    src/PriorityQueue.cpp
)

# --- 1. Handle External Dependencies ---
find_package(SQLite3 REQUIRED)
find_package(OpenSSL)

# --- 2. FetchContent for Dependencies ---
include(FetchContent)

# Argon2
FetchContent_Declare(
  argon2
  GIT_REPOSITORY https://github.com/P-H-C/phc-winner-argon2
  GIT_TAG        20190702
)

set(ARGON2_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(argon2)

# SQLiteCpp
FetchContent_Declare(
  SQLiteCpp
  GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp
  GIT_TAG        3.3.1
)

set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SQLiteCpp)

# --- 3. Build the Main Executable ---
add_executable(cascade ${CASCADE_SOURCES})

# Include local headers (non-library)
# Define the absolute path to the database file
set(DATABASE_ABSOLUTE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/db/cascade.db")

# Pass the database path to the source code as a preprocessor definition
target_compile_definitions(cascade PRIVATE "-DDATABASE_FILE=\"${DATABASE_ABSOLUTE_PATH}\"")

target_include_directories(cascade PRIVATE ${PROJECT_SOURCE_DIR}/include)

# --- 4. Link Everything ---
target_link_libraries(cascade PRIVATE 
    SQLiteCpp
    SQLite::SQLite3
    argon2
    pthread 
    dl 
    c++abi
)

if(OPENSSL_FOUND)
    target_link_libraries(cascade PRIVATE OpenSSL::Crypto)
endif()
