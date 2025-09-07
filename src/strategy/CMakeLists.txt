# Strategy framework library
file(GLOB_RECURSE STRATEGY_SOURCES
    "*.cpp"
    "*.cc"
    "*.cxx"
)

file(GLOB_RECURSE STRATEGY_HEADERS
    "${CMAKE_SOURCE_DIR}/include/cppshares/strategy/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/cppshares/strategy/*.h"
)

add_library(cppshares_strategy ${STRATEGY_SOURCES} ${STRATEGY_HEADERS})

target_include_directories(cppshares_strategy
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cppshares_strategy
    PUBLIC
        cppshares_core
        cppshares_data
        Eigen3::Eigen
)

target_compile_features(cppshares_strategy PUBLIC cxx_std_20)