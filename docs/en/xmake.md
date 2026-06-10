# Using llmapi with xmake

`llmapi` is developed and published mcpp-first, but the library itself is a
plain C++23 module package and can also be consumed from an
[xmake](https://xmake.io/) project.

## Prerequisites

- **C++ Compiler**: GCC 14+, Clang 18+, or MSVC 2022+ with C++23 support
- **Build System**: xmake 3.0.0+
- **Dependencies**: `mbedtls` is resolved automatically by xmake

## Package Usage

Add the mcpplibs xmake repository and require the package in your `xmake.lua`:

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

## Building From Source

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi

xmake
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## Notes

- The xmake package tracks the `mcpplibs-index` repository and may lag behind
  the mcpp package index ([mcpp-index](https://github.com/mcpp-community/mcpp-index)).
- For the mcpp workflow (recommended) see
  [Getting Started](getting-started.md) and the project
  [README](../../README.md).
