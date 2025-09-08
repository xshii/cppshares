# Unit tests
find_package(GTest REQUIRED)

# Collect core tests
file(GLOB CORE_TEST_SOURCES "core/*.cpp")

# Collect data tests
file(GLOB_RECURSE DATA_TEST_SOURCES "data/*.cpp")

# Collect all test sources
set(TEST_SOURCES ${CORE_TEST_SOURCES} ${DATA_TEST_SOURCES})

# Create test executable
add_executable(cppshares_tests ${TEST_SOURCES})

target_link_libraries(cppshares_tests
    PRIVATE
        cppshares_core
        cppshares_data
        cppshares_strategy
        cppshares_backtest
        cppshares_utils
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
)

target_include_directories(cppshares_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(cppshares_tests PRIVATE cxx_std_20)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(cppshares_tests

    # 按模块分组测试
    EXTRA_ARGS --gtest_output=xml:${CMAKE_BINARY_DIR}/test_results.xml
)