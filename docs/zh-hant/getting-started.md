# 快速開始

## 環境需求

- 編譯器：GCC 14+、Clang 18+ 或 MSVC 2022+
- 建置工具：`xmake`
- 依賴：`mbedtls` 由 xmake 自動處理

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

## 從原始碼建置

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi

xmake
xmake run hello_mcpp
```

## 第一個範例

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
    auto resp = client.chat("請用一句話介紹現代 C++。");
    std::cout << resp.text() << '\n';
}
```

## 切換 Provider

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

相容端點：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```
