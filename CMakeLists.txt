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

# --- 1. Define the 'sqlite_orm' library target ---
# INTERFACE means: "I have no source files, just headers/settings to pass on"
add_library(sqlite_orm INTERFACE)

# Tell CMake where to find headers for anyone who uses this library
target_include_directories(sqlite_orm INTERFACE ${PROJECT_SOURCE_DIR}/lib)

# --- 2. Handle Dependencies ---
# sqlite_orm *needs* SQLite3. We link SQLite3 to the *library*, not the app.
# This is "Transitive Dependency": App -> sqlite_orm -> SQLite3
find_package(SQLite3 REQUIRED)
target_link_libraries(sqlite_orm INTERFACE SQLite::SQLite3)

find_package(OpenSSL)

# --- 3. Build the Main Executable ---
file(GLOB SOURCES "src/*.cpp")
add_executable(cascade ${SOURCES})

# Include local headers (non-library)
target_include_directories(cascade PRIVATE ${PROJECT_SOURCE_DIR}/include)

# --- 4. Link Everything ---
# We only link 'sqlite_orm'. Because of the step above, 
# this AUTOMATICALLY links SQLite::SQLite3 as well!
target_link_libraries(cascade PRIVATE 
    sqlite_orm 
    pthread 
    dl 
    c++abi
)

if(OPENSSL_FOUND)
    target_link_libraries(cascade PRIVATE OpenSSL::Crypto)
endif()
