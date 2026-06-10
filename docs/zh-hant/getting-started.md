# 快速開始

## 環境需求

- 建置工具：[mcpp](https://github.com/mcpp-community/mcpp) — 工具鏈與相依
  （如 `tinyhttps`）自動解析
- 也可以在 xmake 專案中使用，見[使用 xmake 整合](xmake.md)

## 從模板建立專案（推薦）

```bash
mcpp new myagent --template llmapi
cd myagent
mcpp run
```

列出函式庫提供的模板：

```bash
mcpp new --list-templates llmapi
```

## 在既有專案中引入

```bash
mcpp add llmapi
```

或在 `mcpp.toml` 中宣告：

```toml
[dependencies.mcpplibs]
llmapi = "0.2.7"
```

## 從原始碼建置

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi
mcpp build
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
