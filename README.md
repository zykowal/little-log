# Logger Library - 100% Test Coverage

## 概述
这是一个使用C++14实现的动态链接库，提供完整的日志记录功能，包括文件轮转、备份管理等特性。

## 架构设计
- **依赖注入**: 使用接口抽象文件系统操作，支持Mock测试
- **单一职责**: Logger负责日志逻辑，FileSystem负责文件操作
- **可测试性**: 通过依赖注入实现100%代码覆盖率

## 文件结构
```
├── logger.h                    # Logger类接口
├── logger.cpp                  # Logger实现
├── file_system_interface.h     # 文件系统接口
├── real_file_system.h         # 真实文件系统实现
├── real_file_system.cpp       # 真实文件系统实现
├── test_logger.cpp            # 完整测试套件
├── liblogger.dylib            # 编译后的动态库
└── Makefile                   # 构建配置
```

## 核心功能
1. **init()** - 初始化日志配置
2. **enableTerminal()/disableTerminal()** - 控制终端输出
3. **logMsg()** - 记录消息到文件和终端
4. **logFact()** - 强制输出到终端
5. **backup()** - 创建日志备份
6. **自动轮转** - 文件大小超限时自动轮转

## 测试覆盖率
- **logger.cpp**: 100% 覆盖率 (89/89 行)
- **real_file_system.cpp**: 100% 覆盖率 (19/19 行)

## 测试类型
1. **单元测试** (Mock): 测试Logger逻辑，使用MockFileSystem
2. **组件测试**: 测试RealFileSystem的所有方法
3. **集成测试**: 端到端测试完整工作流程

## 编译和测试
```bash
# 编译库
make

# 运行测试
make test

# 生成覆盖率报告
make coverage

# 清理
make clean
```

## 测试用例
- 目录创建测试
- 终端输出控制测试
- 文件写入测试
- 日志轮转测试
- 备份功能测试
- 大小限制测试
- 边界条件测试
- 错误处理测试

