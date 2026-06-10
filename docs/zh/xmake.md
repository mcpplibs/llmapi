# 使用 xmake 集成 llmapi

`llmapi` 以 mcpp 生态为主进行开发与发布,但库本身是标准的 C++23 模块包,
也可以在 [xmake](https://xmake.io/) 项目中使用。

## 前置条件

- **C++ 编译器**:支持 C++23 的 GCC 14+、Clang 18+ 或 MSVC 2022+
- **构建系统**:xmake 3.0.0+
- **依赖**:`mbedtls` 由 xmake 自动解析

## 包管理使用

在 `xmake.lua` 中添加 mcpplibs 的 xmake 仓库并引入包:

```lua
add_repositories("mcpplibs-index https://github.com/mcpplibs/mcpplibs-index.git")
add_requires("llmapi 0.1.0")

target("demo")
    set_kind("binary")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_files("src/*.cpp")
    add_packages("llmapi")
```

## 源码构建

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi

xmake
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## 说明

- xmake 包跟踪 `mcpplibs-index` 仓库,版本可能滞后于 mcpp 包索引
  ([mcpp-index](https://github.com/mcpp-community/mcpp-index))。
- 推荐的 mcpp 工作流见[快速上手](getting-started.md)与项目
  [README](../../README.zh.md)。
