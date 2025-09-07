# Backtesting engine library
file(GLOB_RECURSE BACKTEST_SOURCES
    "*.cpp"
    "*.cc"
    "*.cxx"
)

file(GLOB_RECURSE BACKTEST_HEADERS
    "${CMAKE_SOURCE_DIR}/include/cppshares/backtest/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/cppshares/backtest/*.h"
)

add_library(cppshares_backtest ${BACKTEST_SOURCES} ${BACKTEST_HEADERS})

target_include_directories(cppshares_backtest
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cppshares_backtest
    PUBLIC
        cppshares_core
        cppshares_strategy
        cppshares_data
)

target_compile_features(cppshares_backtest PUBLIC cxx_std_20)