# 使用 xmake 整合 llmapi

`llmapi` 以 mcpp 生態為主進行開發與發佈,但函式庫本身是標準的 C++23 模組套件,
也可以在 [xmake](https://xmake.io/) 專案中使用。

## 前置條件

- **C++ 編譯器**:支援 C++23 的 GCC 14+、Clang 18+ 或 MSVC 2022+
- **建置系統**:xmake 3.0.0+
- **相依**:`mbedtls` 由 xmake 自動解析

## 套件管理使用

在 `xmake.lua` 中加入 mcpplibs 的 xmake 倉庫並引入套件:

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

## 原始碼建置

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi

xmake
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## 說明

- xmake 套件追蹤 `mcpplibs-index` 倉庫,版本可能落後於 mcpp 套件索引
  ([mcpp-index](https://github.com/mcpp-community/mcpp-index))。
- 推薦的 mcpp 工作流程見[快速上手](getting-started.md)與專案
  [README](../../README.zh.hant.md)。
