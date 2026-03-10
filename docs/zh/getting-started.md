# 快速开始

## 环境要求

- 编译器：GCC 14+、Clang 18+ 或 MSVC 2022+
- 构建工具：`xmake`
- 依赖：`mbedtls` 由 xmake 自动处理

## 使用 xmake 引入

```lua
add_repositories("mcpplibs-index git@github.com:mcpplibs/mcpplibs-index.git")
add_requires("llmapi 0.0.2")

target("myapp")
    set_kind("binary")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_files("src/*.cpp")
    add_packages("llmapi")
```

## 从源码构建

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi

xmake
xmake run hello_mcpp
```

## 第一个例子

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    client.system("You are a helpful assistant.");
    auto resp = client.chat("请用一句话介绍现代 C++。");
    std::cout << resp.text() << '\n';
}
```

## 切换 Provider

OpenAI：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
```

Anthropic：

```cpp
auto client = Client(AnthropicConfig{
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model = "claude-sonnet-4-20250514",
});
```

兼容端点：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```
