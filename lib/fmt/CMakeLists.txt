cmake_minimum_required(VERSION 3.10)

project(fmt CXX)

# Define source files (based on original line 304-313)
set(FMT_SOURCES
    src/format.cc
    src/os.cc
)

# Create static library
add_library(fmt STATIC ${FMT_SOURCES})

# Require C++11 (same as original line 326)
target_compile_features(fmt PUBLIC cxx_std_11)

# Set include directories (based on original line 331-333)
target_include_directories(fmt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Create fmt::fmt alias (required - original line 309)
add_library(fmt::fmt ALIAS fmt)
