# llmapi

> Modern C++ LLM API client with openai-compatible support

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![C API](https://img.shields.io/badge/C_API-ok-green.svg)](https://en.cppreference.com/w/cpp/23)
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
- **C 語言 API** - 完整的 C 語言支援，物件導向風格
- **提供商無關** - OpenAI、Poe 及相容端點
- **C++17 純標頭檔** - 原生支援 C++17，僅需 `#include <llmapi.hpp>`（API 與 C++23 一致）

## 快速開始

### C++ API

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

### C API

```c
#include <stdio.h>

#include "llmapi.h"

void stream_print(const char* s, size_t len, void* data) {
    printf("%.*s", (int)len, s);
    fflush(stdout);
}

int main(void) {
    llmapi_client_t* c = llmapi_client_create(getenv("OPENAI_API_KEY"), LLMAPI_URL_POE);

    c->set_model(c, "gpt-5");
    c->add_system_message(c, "You are a helpful assistant.");
    c->add_user_message(c, "In one sentence, introduce modern C++. 並給出中文翻譯");
    c->request_stream(c, stream_print, NULL);
    
    c->destroy(c);
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
add_repositories("mcpplibs-index git@github.com:mcpplibs/mcpplibs-index.git")

-- 1 - 新增需要的函式庫和版本
add_requires("llmapi 0.0.1")
-- add_requires("llmapi 0.0.1", configs = { capi = true }) -- 如果使用 C API
```

> More: [mcpplibs-index](https://github.com/mcpplibs/mcpplibs-index)

### cmake

```
todo...
```

## 📚 API 參考

**C++ 核心方法：**
- `model(name)` - 設定模型
- `user/system/assistant(content)` - 新增訊息
- `request()` - 非串流（回傳 JSON）
- `request(callback)` - 串流輸出
- `getAnswer()` - 取得最後的助手回覆
- `getMessages()` - 取得對話歷史
- `clear()` - 清空歷史

**C API：** 所有方法透過函式指標存取 (`client->method(client, ...)`)

## 📄 授權條款

Apache-2.0 - 詳見 [LICENSE](LICENSE)
