# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

CppShares is a modern C++ quantitative trading system featuring:
- **Multi-API data aggregation** with failover and load balancing
- **Type-safe enumerations** for symbols, markets, and data types
- **Modular architecture** with core trading engine, strategy framework, and backtesting
- **Mixed logging system** combining spdlog text logs with high-performance binary logs
- **Web API interface** for real-time monitoring and control
- **Comprehensive testing** using Google Test framework

## Build System & Development Commands

### Prerequisites
- CMake 3.20+
- Modern C++ compiler supporting C++20
- Conan package manager for dependencies

### Core Commands
```bash
# Install dependencies
conan install . --build=missing -s build_type=Debug

# Configure and build (uses CMakePresets.json)
cmake --preset debug
cmake --build --preset debug

# Run tests
ctest --preset debug --verbose

# Run application
./build/debug/cppshares

# Clean build
rm -rf build/
```

### VSCode Integration
Use `Ctrl+Shift+P` → `Tasks: Run Build Task` for integrated building, or:
- **Build Debug** (default build task)
- **Run Tests** - Build and run all unit tests  
- **Run Application** - Build and run main program
- **Build & Test** - Combined build and test pipeline

## Project Architecture

### Directory Structure
```
cppshares/
├── src/                          # Source implementations
│   ├── core/                     # Trading engine core
│   ├── data/                     # Data providers and aggregation
│   ├── strategy/                 # Strategy framework
│   ├── backtest/                 # Backtesting engine
│   ├── api/                      # Web API server
│   └── utils/                    # Logging, config, math utilities
├── include/cppshares/            # Public headers (same structure)
├── tests/                        # Unit tests with GTest
├── cmake/conan/                  # Conan-generated CMake files
├── .vscode/                      # VSCode tasks and launch configs
└── docs/                         # Documentation
```

### Key Modules

#### Data Layer (`src/data/`, `include/cppshares/data/`)
- **Multi-API aggregation** supporting Sina, Tencent, and Netease financial APIs
- **Type-safe data types**: `Symbol`, `Market`, `SecurityType`, `KlinePeriod` 
- **Failover strategies**: `FailoverStrategy`, `RoundRobinStrategy`, `WeightedRandomStrategy`
- **Health monitoring** and automatic provider switching
- **Format conversion** utilities for different API providers

#### Core Engine (`src/core/`, `include/cppshares/core/`)  
- **TradingEngine** - Main trading system coordinator
- **Market data structures** - `MarketTick`, `OHLCV` with timestamp precision
- **Order management** - Order lifecycle and execution tracking

#### Strategy Framework (`src/strategy/`, `include/cppshares/strategy/`)
- **BaseStrategy** - Abstract strategy interface with lifecycle methods
- **Plugin architecture** - Strategies as loadable modules  
- **Technical indicators** - Built-in math utilities (RSI, MACD, Bollinger Bands)

#### Utilities (`src/utils/`, `include/cppshares/utils/`)
- **HybridLogger** - Combines spdlog (text) + custom binary logging
- **Binary log formats** - `MarketDataRecord`, `OrderRecord`, `StrategySignalRecord`
- **CSV export** - `BinaryLogReader` for post-analysis
- **Math utilities** - All functions use `static inline` for performance

#### Testing (`tests/`)
- **Comprehensive coverage** - Unit tests for all major components
- **Mock providers** - Using Google Mock for testing data strategies
- **Performance benchmarks** - Built-in performance testing
- **Integration tests** - Multi-provider coordination testing

## Modern C++ Features Used

### C++20 Features
- **Concepts** - Strategy interface constraints
- **Ranges** - Data pipeline processing  
- **std::format** - Type-safe string formatting
- **Coroutines** - Async data fetching (planned)
- **std::span** - Safe array access
- **std::jthread** - Thread pool management

### Design Patterns
- **Strategy Pattern** - Pluggable data acquisition strategies
- **Factory Pattern** - Provider instantiation  
- **Observer Pattern** - Market data event handling
- **Template Metaprogramming** - Type-safe enum conversions

## Dependencies (via Conan)

### Core Libraries
- **nlohmann_json** - JSON parsing for API responses
- **cpp-httplib** - HTTP client for data providers (without OpenSSL for dev)
- **sqlite3** - Embedded database for data storage
- **spdlog** - High-performance logging (header-only mode)
- **eigen** - Linear algebra for strategy calculations
- **gtest** - Unit testing framework

### Build Configuration
- **Development mode** - OpenSSL disabled for faster builds
- **Thread-safe SQLite** - Multi-threaded data access
- **Header-only spdlog** - Reduced binary size
- **GTest with GMock** - Comprehensive testing capabilities

## Development Workflow

### Typical Development Cycle
1. **Data Provider Development** - Implement new `DataProvider` subclass
2. **Strategy Implementation** - Extend `BaseStrategy` for trading logic
3. **Unit Testing** - Add tests in corresponding `tests/` subdirectory
4. **Integration** - Register with `DataAggregator` or `TradingEngine`
5. **Backtesting** - Validate with historical data

### Performance Considerations
- **Static inline functions** - Math utilities optimized for speed
- **Binary logging** - High-frequency data recording without serialization overhead
- **Memory pools** - Planned for order/tick data allocation
- **Lock-free structures** - For real-time market data handling

## Testing Strategy

Run specific test suites:
```bash
# All tests
cmake --build --preset debug && ctest --preset debug

# Specific test categories  
./build/debug/cppshares_tests --gtest_filter="DataTypesTest.*"
./build/debug/cppshares_tests --gtest_filter="DataStrategyTest.*"
./build/debug/cppshares_tests --gtest_filter="ProvidersTest.*"
```

## Configuration Files

### CMakePresets.json
- **debug** - Development with AddressSanitizer
- **release** - Production optimized build
- **dev** - RelWithDebInfo for daily development  
- **minimal** - Core-only build

### VSCode Configuration
- **tasks.json** - Build, test, and run tasks
- **launch.json** - Debug application and specific tests
- Integrated with CMake presets for seamless development

## Code Generation Guidelines for Claude

### Code Formatting Standards
**CRITICAL**: This project uses clang-format. ALL generated code MUST be format-compliant from the start.

#### Pre-Generation Checklist
1. **Read .clang-format configuration** before writing any code
2. **Analyze existing code patterns** in the target file/directory  
3. **Check line length limits** (typically 100 characters)
4. **Verify indentation consistency** with surrounding code

#### Formatting Rules to Follow
- **Line Length**: Keep lines under 100 characters
- **Long Statements**: Break long expressions at logical points
- **Output Streams**: Split long cout statements into multiple shorter ones
- **Function Calls**: Break long parameter lists across lines
- **Logical Operators**: Place operators at end of line when breaking
- **Trailing Spaces**: Never leave trailing whitespace

#### Common Patterns

**❌ AVOID - Long cout statements:**
```cpp
std::cout << "Very long message with lots of variables " << var1 << " and more text " << var2 << " ending here" << std::endl;
```

**✅ PREFER - Split cout statements:**
```cpp
std::cout << "Message with variable: " << var1 << std::endl;
std::cout << "Additional info: " << var2 << std::endl;
```

**❌ AVOID - Long function calls:**
```cpp
auto result = some_function(very_long_parameter_name, another_long_parameter, third_parameter);
```

**✅ PREFER - Multi-line function calls:**
```cpp
auto result = some_function(very_long_parameter_name,
                           another_long_parameter,
                           third_parameter);
```

**❌ AVOID - Long conditional expressions:**
```cpp
if (condition1 && condition2 && condition3 && condition4) {
```

**✅ PREFER - Multi-line conditionals:**
```cpp
if (condition1 && condition2 &&
    condition3 && condition4) {
```

#### Testing Code Patterns
- **CI Detection**: Use consistent environment variable checking
- **Test Data**: Maintain test symbols/data within functions as const vectors
- **Network Tests**: Always check CI environment before network calls
- **Output**: Use concise, informative test output messages

#### Verification Steps
Before completing code generation:
1. **Visual scan** - Check for obviously long lines
2. **Consistent indentation** - Verify alignment with existing code  
3. **Logical breaks** - Ensure line breaks are at natural points
4. **No trailing spaces** - Clean line endings

### Automated Formatting
If formatting violations occur:
```bash
# Format specific file
clang-format -i path/to/file.cpp

# Format multiple files  
clang-format -i src/**/*.cpp include/**/*.hpp tests/**/*.cpp
```

**Remember**: It's better to write correctly formatted code initially than to fix it post-generation. This saves time and reduces commit noise.