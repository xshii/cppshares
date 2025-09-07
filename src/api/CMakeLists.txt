# Web API server library
file(GLOB_RECURSE API_SOURCES
    "*.cpp"
    "*.cc"
    "*.cxx"
)

file(GLOB_RECURSE API_HEADERS
    "${CMAKE_SOURCE_DIR}/include/cppshares/api/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/cppshares/api/*.h"
)

add_library(cppshares_api ${API_SOURCES} ${API_HEADERS})

target_include_directories(cppshares_api
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cppshares_api
    PUBLIC
        cppshares_core
        cppshares_strategy
        cppshares_backtest
        httplib::httplib
)

target_compile_features(cppshares_api PUBLIC cxx_std_20)