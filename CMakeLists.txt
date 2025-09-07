cmake_minimum_required(VERSION 3.20)

# Include Conan toolchain if it exists
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/conan/conan_toolchain.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/conan/conan_toolchain.cmake")
endif()

project(cppshares
    VERSION 1.0.0
    DESCRIPTION "Modern C++ Quantitative Trading System"
    LANGUAGES CXX
)

# Modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -fsanitize=address)
        add_link_options(-fsanitize=address)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od)
    else()
        add_compile_options(/O2 /DNDEBUG)
    endif()
endif()

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_WEB_API "Build web API server" ON)
option(BUILD_EXAMPLES "Build example strategies" ON)

# Conan generated files location
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/conan)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/cmake/conan)

# Find packages
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(httplib REQUIRED) 
find_package(SQLite3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GTest REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src/utils)
add_subdirectory(src/core)
add_subdirectory(src/data)
add_subdirectory(src/strategy)
add_subdirectory(src/backtest)

if(BUILD_WEB_API)
    add_subdirectory(src/api)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Main executable
add_executable(cppshares src/main.cpp)
target_link_libraries(cppshares
    PRIVATE
        cppshares_core
        cppshares_data
        cppshares_strategy
        cppshares_backtest
        cppshares_utils
        Threads::Threads
)

if(BUILD_WEB_API)
    target_link_libraries(cppshares PRIVATE cppshares_api)
endif()

# Installation
install(TARGETS cppshares
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# CPack configuration
set(CPACK_PACKAGE_NAME "CppShares")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
include(CPack)