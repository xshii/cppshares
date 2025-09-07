# Data management library
file(GLOB_RECURSE DATA_SOURCES
    "*.cpp"
    "*.cc"
    "*.cxx"
)

file(GLOB_RECURSE DATA_HEADERS
    "${CMAKE_SOURCE_DIR}/include/cppshares/data/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/cppshares/data/*.h"
)

add_library(cppshares_data ${DATA_SOURCES} ${DATA_HEADERS})

target_include_directories(cppshares_data
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cppshares_data
    PUBLIC
        cppshares_utils
        SQLite::SQLite3
        httplib::httplib
        nlohmann_json::nlohmann_json
)

target_compile_features(cppshares_data PUBLIC cxx_std_20)