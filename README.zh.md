# llmapi

> Modern C++ LLM API client with openai-compatible support

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![C API](https://img.shields.io/badge/C_API-ok-green.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI_API-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| [English](README.md) - 简体中文 - [繁體中文](README.zh.hant.md) |
|:---:|
| [完整文档](docs/) - [C++ API](docs/cpp-api.md) - [C API](docs/c-api.md) - [示例](docs/examples.md) |

简洁、类型安全的 LLM API 客户端，使用 C++23 模块。流式接口设计，零成本抽象。支持 OpenAI、Poe、DeepSeek 及兼容端点。

## ✨ 特性

- **C++23 模块** - `import mcpplibs.llmapi`
- **自动保存历史** - 对话历史自动管理
- **类型安全流式** - 概念约束的回调函数
- **流式接口** - 可链式调用的方法
- **C 语言 API** - 完整的 C 语言支持，面向对象风格
- **提供商无关** - OpenAI、Poe 及兼容端点

## 快速开始

### C++ API

```cpp
import std;
import mcpplibs.llmapi;

int main() {
    using namespace mcpplibs;
    
    llmapi::Client client(std::getenv("OPENAI_API_KEY"), llmapi::URL::Poe);

    client.model("gpt-5")
          .system("You are a helpful assistant.")
          .user("In one sentence, introduce modern C++. 并给出中文翻译")
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
    c->add_user_message(c, "In one sentence, introduce modern C++. 并给出中文翻译");
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
llmapi::Client client(apiKey, "https://custom.com");   // 自定义
```

## 🛠️ 构建

```bash
xmake              # 构建
xmake run basic    # 运行示例（需要先配置 OPENAI_API_KEY）
```

## 📦 在构建工具中使用

### xmake

```lua
-- 0 - 添加 mcpplibs 索引仓库
add_repositories("mcpplibs-index git@github.com:mcpplibs/mcpplibs-index.git")

-- 1 - 添加需要的库和版本
add_requires("llmapi 0.0.1")
-- add_requires("llmapi 0.0.1", configs = { capi = true }) -- 如果使用 C API
```

> More: [mcpplibs-index](https://github.com/mcpplibs/mcpplibs-index)

### cmake

```
todo...
```

## 📚 API 参考

**C++ 核心方法：**
- `model(name)` - 设置模型
- `user/system/assistant(content)` - 添加消息
- `request()` - 非流式（返回 JSON）
- `request(callback)` - 流式输出
- `getAnswer()` - 获取最后的助手回复
- `getMessages()` - 获取对话历史
- `clear()` - 清空历史

**C API：** 所有方法通过函数指针访问 (`client->method(client, ...)`)

## 📄 许可证

Apache-2.0 - 详见 [LICENSE](LICENSE)
