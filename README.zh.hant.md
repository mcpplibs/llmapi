# llmapi

> Modern C++ LLM API client with openai-compatible support

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI_API-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| [English](README.md) - [简体中文](README.zh.md) - 繁體中文 |
|:---:|
| [完整文件](docs/) - [C++ API](docs/cpp-api.md) - [C API](docs/c-api.md) - [範例](docs/examples.md) |

簡潔、型別安全的 LLM API 客戶端，使用 C++23 模組。流式介面設計，零成本抽象。支援 OpenAI、Poe、DeepSeek 及相容端點。

## ✨ 特性

- **C++23 模組** - `import mcpplibs.llmapi`
- **自動儲存歷史** - 對話歷史自動管理
- **型別安全串流** - 概念約束的回呼函式
- **流式介面** - 可鏈式呼叫的方法
- **提供商無關** - OpenAI、Poe 及相容端點

## 快速開始


```cpp
import std;
import mcpplibs.llmapi;

int main() {
    using namespace mcpplibs;
    
    llmapi::Client client(std::getenv("OPENAI_API_KEY"), llmapi::URL::Poe);

    client.model("gpt-5")
          .system("You are a helpful assistant.")
          .user("In one sentence, introduce modern C++. 並給出中文翻譯")
          .request([](std::string_view chunk) {
                std::print("{}", chunk);
                std::cout.flush();
          });

    return 0;
}
```

### 模型 / 提供商

```cpp
llmapi::Client client(apiKey, llmapi::URL::OpenAI);    // OpenAI
llmapi::Client client(apiKey, llmapi::URL::Poe);       // Poe
llmapi::Client client(apiKey, llmapi::URL::DeepSeek);  // Deepseek
llmapi::Client client(apiKey, "https://custom.com");   // 自訂
```

## 🛠️ 建置

```bash
xmake              # 建置
xmake run basic    # 執行範例（需先設定 OPENAI_API_KEY）
```

## 📦 在建置工具中使用

### xmake

```lua
-- 0 - 新增 mcpplibs 索引倉庫
add_repositories("mcpplibs-index https://github.com/mcpplibs/llmapi.git")

-- 1 - 新增需要的函式庫和版本
add_requires("llmapi 0.0.2")
```

> More: [mcpplibs-index](https://github.com/mcpplibs/mcpplibs-index)

### cmake

```
todo...
```

## 📄 授權條款

Apache-2.0 - 詳見 [LICENSE](LICENSE)
