# CppShares - 现代C++量化交易系统

🚀 基于现代C++20构建的高性能量化交易后端系统

## 📋 特性

- 🏗️ **模块化架构** - 清晰分离的核心/数据/策略/回测/API模块
- ⚡ **高性能设计** - 枚举数值存储、二进制日志、零拷贝数据结构
- 📊 **多数据源聚合** - 支持新浪/腾讯/网易财经API，带故障转移
- 🔧 **现代C++** - 使用C++20特性：concepts、ranges、coroutines
- 🧪 **完整测试** - 基于Google Test的单元测试覆盖
- 🛠️ **开发友好** - CMake构建系统、VSCode集成、Conan包管理

## 🚀 快速开始

### 环境要求

- C++20 编译器 (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.20+
- Conan 2.0+

### 构建步骤

```bash
# 1. 克隆项目
git clone https://github.com/xshii/cppshares.git
cd cppshares

# 2. 安装依赖
conan install . --output-folder=cmake/conan --build=missing

# 3. 配置构建
cmake --preset debug

# 4. 编译
cmake --build build/debug

# 5. 运行测试
ctest --preset debug
```

### VSCode 开发

项目已配置好VSCode开发环境：
- `Ctrl+Shift+P` → `Tasks: Run Task` → `Build Debug`
- `F5` 启动调试

## 📁 项目结构

```
├── include/cppshares/          # 头文件
│   ├── core/                   # 核心引擎
│   ├── data/                   # 数据获取与管理
│   ├── strategy/               # 交易策略
│   ├── backtest/               # 回测引擎
│   ├── api/                    # Web API接口
│   └── utils/                  # 工具库
├── src/                        # 源文件
├── tests/                      # 单元测试
├── docs/                       # 文档
└── cmake/                      # 构建配置
```

## 🎯 核心模块

### 数据层 (Data)
- **多API聚合**: 新浪、腾讯、网易财经API
- **故障转移**: Failover、RoundRobin、WeightedRandom策略
- **类型安全**: 强类型枚举，避免字符串错误

### 策略层 (Strategy)  
- **基类框架**: 统一的策略接口
- **事件驱动**: 基于市场数据事件的策略执行

### 回测引擎 (Backtest)
- **历史数据回放**: 基于真实市场数据
- **性能分析**: 收益率、最大回撤、夏普比率

### 日志系统 (Logging)
- **混合日志**: 文本日志 + 高频二进制日志
- **高性能**: 缓冲写入，微秒级时间戳

## 🔧 配置

主要配置在 `conanfile.txt`:

```ini
[requires]
nlohmann_json/3.11.3    # JSON处理
cpp-httplib/0.18.0       # HTTP客户端
sqlite3/3.43.2          # 数据存储
spdlog/1.12.0           # 日志库
eigen/3.4.0             # 数学计算
gtest/1.14.0            # 单元测试
```

## 🧪 测试

```bash
# 运行所有测试
cmake --build build/debug --target test

# 详细输出
ctest --preset debug --verbose

# 运行特定测试
./build/debug/cppshares_tests --gtest_filter="DataTypesTest.*"
```

## 📈 性能优化

- ✅ 使用数值枚举替代字符串，避免运行时转换
- ✅ 二进制日志格式，高频数据零开销记录
- ✅ 内存池和对象复用
- ✅ 编译时多态优化

## 🛣️ 开发路线

- [ ] 完善实时数据解析逻辑
- [ ] 实现策略回测完整流程  
- [ ] 添加Web Dashboard界面
- [ ] 支持更多数据源和交易所
- [ ] 机器学习策略集成

## 📝 许可证

MIT License - 详见 [LICENSE](LICENSE)

## 🤝 贡献

欢迎提交Issue和Pull Request！

---

**注**: 本项目仅用于学习和研究目的，不构成投资建议。