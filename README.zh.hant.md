# llmapi

> 使用 C++23 模組建構的現代 LLM 客戶端 — `import` 即聊天 · OpenAI 相容 · 模板開箱即用

[![Release](https://img.shields.io/github/v/release/mcpplibs/llmapi)](https://github.com/mcpplibs/llmapi/releases)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| [English](README.md) - [简体中文](README.zh.md) - 繁體中文 |
|:---:|
| [mcpp 建置工具](https://github.com/mcpp-community/mcpp) · [套件索引 mcpp-index](https://github.com/mcpp-community/mcpp-index) · [文件導覽](docs/README.md) · [Issues](https://github.com/mcpplibs/llmapi/issues) |

`llmapi` 提供型別化的 `Client<Provider>` API，涵蓋聊天、串流輸出、embeddings、工具呼叫與對話持久化。預設別名 `Config` 對應 OpenAI 風格設定，常見情況下不需要顯式寫出 `openai::OpenAI(...)`。

## 快速開始(mcpp)

```bash
mcpp new myagent --template llmapi && cd myagent
export OPENAI_API_KEY="sk-..."
mcpp run
```

模板隨函式庫發佈、版本自動對齊:

```bash
mcpp new --list-templates llmapi              # 列出函式庫提供的模板
mcpp new mybot --template llmapi:chat        # 互動式串流聊天 CLI
mcpp new mybot --template llmapi:anthropic   # Anthropic Provider
mcpp new mybot --template llmapi:deepseek    # OpenAI 相容端點(DeepSeek)
```

或在既有 mcpp 專案中接入:

```bash
mcpp add llmapi
```

```toml
[dependencies.mcpplibs]
llmapi = "0.2.8"
```

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::println(stderr, "OPENAI_API_KEY not set");
        return 1;
    }

    auto client = Client(Config{
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    });

    client.system("You are a concise assistant.");
    auto resp = client.chat("用兩句話解釋 C++23 模組的價值。");

    std::println("{}", resp.text());
    return 0;
}
```

## 特性

- C++23 模組：`import mcpplibs.llmapi`
- 強型別訊息、工具與回應結構
- 同步、非同步、串流聊天介面
- OpenAI Provider 支援 embeddings
- 支援儲存 / 載入對話歷史
- 可透過 `baseUrl` 存取 OpenAI 相容端點
- 專案模板：`mcpp new <name> --template llmapi[:<template>]`

## 模板

| 模板 | 說明 |
|---|---|
| `openai`(預設) | 最小 OpenAI 聊天 — 一問一答 |
| `chat` | 互動式串流聊天 CLI(OpenAI) |
| `anthropic` | Anthropic(Claude)聊天 |
| `deepseek` | 透過 `baseUrl` 走 OpenAI 相容端點(DeepSeek) |

## Provider

- `Config`：`openai::Config` 的匯出別名，預設走 OpenAI 風格
- `openai::OpenAI`：OpenAI 聊天、串流、工具呼叫、embeddings
- `AnthropicConfig` / `anthropic::Anthropic`：Anthropic 聊天與串流

相容端點範例：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## 原始碼建置

```bash
git clone https://github.com/mcpplibs/llmapi.git && cd llmapi
mcpp build
```

## 文件

- [快速上手](docs/zh-hant/getting-started.md)
- [Provider 設定](docs/zh-hant/providers.md)
- [C++ API 指南](docs/zh-hant/cpp-api.md)
- [使用 xmake 整合 llmapi](docs/zh-hant/xmake.md)
- [文件導覽](docs/README.md)

## 授權

Apache-2.0，詳見 [LICENSE](LICENSE)
