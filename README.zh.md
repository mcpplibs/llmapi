# llmapi

> Modern C++ LLM API client with openai-compatible support

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
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
- **提供商无关** - OpenAI、Poe 及兼容端点

## 快速开始

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

### 模型 / 提供商

```cpp
llmapi::Client client(apiKey, llmapi::URL::OpenAI);    // OpenAI
llmapi::Client client(apiKey, llmapi::URL::Poe);       // Poe
llmapi::Client client(apiKey, llmapi::URL::DeepSeek);  // Deepseek
llmapi::Client client(apiKey, "https://custom.com");   // 自定义
```

## 构建

```bash
xmake              # 构建
xmake run basic    # 运行示例（需要先配置 OPENAI_API_KEY）
```

## 在构建工具中使用

### xmake

```lua
-- 0 - 添加 mcpplibs 索引仓库
add_repositories("mcpplibs-index https://github.com/mcpplibs/llmapi.git")

-- 1 - 添加需要的库和版本
add_requires("llmapi 0.0.2")
```

> More: [mcpplibs-index](https://github.com/mcpplibs/mcpplibs-index)

### cmake

```
todo...
```

## 📄 许可证

Apache-2.0 - 详见 [LICENSE](LICENSE)
