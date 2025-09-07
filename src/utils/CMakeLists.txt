# Utils library
file(GLOB_RECURSE UTILS_SOURCES
    "*.cpp"
    "*.cc"
    "*.cxx"
)

file(GLOB_RECURSE UTILS_HEADERS
    "${CMAKE_SOURCE_DIR}/include/cppshares/utils/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/cppshares/utils/*.h"
)

add_library(cppshares_utils ${UTILS_SOURCES} ${UTILS_HEADERS})

target_include_directories(cppshares_utils
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cppshares_utils
    PUBLIC
        spdlog::spdlog_header_only
        fmt::fmt
        nlohmann_json::nlohmann_json
)

# 确保目标属性被正确传播
set_target_properties(cppshares_utils PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_compile_features(cppshares_utils PUBLIC cxx_std_20)