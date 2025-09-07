# 构建配置说明

## CMakePresets.json 配置说明

### 构建预设 (Configure Presets)

#### debug 预设
- **用途**: 开发调试版本
- **特性**: 
  - 启用AddressSanitizer检测内存错误
  - 包含调试符号和所有功能模块
  - 生成compile_commands.json供IDE使用

#### release 预设  
- **用途**: 生产环境优化版本
- **特性**:
  - 最高优化等级，关闭调试符号
  - 禁用测试和示例以减小体积
  - 适用于部署环境

#### dev 预设
- **用途**: 日常开发版本
- **特性**:
  - RelWithDebInfo模式，保留调试信息但有一定优化
  - 包含测试但无内存检查器，编译速度较快

#### minimal 预设
- **用途**: 最小核心功能版本
- **特性**:
  - 仅编译核心交易引擎
  - 禁用Web API、测试、示例
  - 适用于嵌入式或资源受限环境

### 构建选项说明

- `BUILD_TESTS`: 是否编译单元测试
- `BUILD_WEB_API`: 是否编译Web API服务器  
- `BUILD_EXAMPLES`: 是否编译示例策略
- `CMAKE_EXPORT_COMPILE_COMMANDS`: 生成编译命令数据库供IDE使用

## conanfile.txt 依赖说明

### 版本选择原则
- **稳定性**: 选择经过充分测试的稳定版本
- **兼容性**: 确保依赖间版本兼容
- **性能**: 优先选择性能优化的版本

### 关键配置项
- `sqlite3:threadsafe=True`: 启用线程安全，支持多线程数据库访问
- `spdlog:header_only=True`: 仅头文件模式，减少链接复杂度
- `httplib:with_openssl=True`: 支持HTTPS加密通信
- `httplib:with_zlib=True`: 启用HTTP压缩，减少网络传输开销

## 使用示例

```bash
# 配置调试版本
cmake --preset debug

# 构建调试版本  
cmake --build --preset debug

# 运行测试
ctest --preset debug

# 配置生产版本
cmake --preset release
cmake --build --preset release
```