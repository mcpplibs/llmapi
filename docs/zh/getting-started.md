# 快速开始

## 环境要求

- 构建工具：[mcpp](https://github.com/mcpp-community/mcpp) — 工具链与依赖
  （如 `tinyhttps`）自动解析
- 也可以在 xmake 项目中使用，见[使用 xmake 集成](xmake.md)

## 从模板创建项目（推荐）

```bash
mcpp new myagent --template llmapi
cd myagent
mcpp run
```

列出库提供的模板：

```bash
mcpp new --list-templates llmapi
```

## 在已有项目中引入

```bash
mcpp add llmapi
```

或在 `mcpp.toml` 中声明：

```toml
[dependencies.mcpplibs]
llmapi = "0.2.7"
```

## 从源码构建

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi
mcpp build
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
