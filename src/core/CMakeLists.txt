# Core trading engine library
file(GLOB_RECURSE CORE_SOURCES
    "*.cpp"
    "*.cc"
    "*.cxx"
)

file(GLOB_RECURSE CORE_HEADERS
    "${CMAKE_SOURCE_DIR}/include/cppshares/core/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/cppshares/core/*.h"
)

add_library(cppshares_core ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(cppshares_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cppshares_core
    PUBLIC
        cppshares_utils
        Threads::Threads
)

target_compile_features(cppshares_core PUBLIC cxx_std_20)