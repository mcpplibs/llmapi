# llmapi

> Modern C++ LLM API client with openai-compatible support

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI_API-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| English - [简体中文](README.zh.md) - [繁體中文](README.zh.hant.md) |
|:---:|
| [Documentation](docs/) - [C++ API](docs/cpp-api.md) - [C API](docs/c-api.md) - [Examples](docs/examples.md) |

Clean, type-safe LLM API client using C++23 modules. Fluent interface with zero-cost abstractions. Works with OpenAI, Poe, DeepSeek and compatible endpoints.

## ✨ Features

- **C++23 Modules** - `import mcpplibs.llmapi`
- **Auto-Save History** - Conversation history managed automatically
- **Type-Safe Streaming** - Concept-constrained callbacks
- **Fluent Interface** - Chainable methods
- **Provider Agnostic** - OpenAI, Poe, and compatible endpoints

## Quick Start

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

### Models / Providers

```cpp
llmapi::Client client(apiKey, llmapi::URL::OpenAI);    // OpenAI
llmapi::Client client(apiKey, llmapi::URL::Poe);       // Poe
llmapi::Client client(apiKey, llmapi::URL::DeepSeek);  // Deepseek
llmapi::Client client(apiKey, "https://custom.com");   // Custom
```

## Building

```bash
xmake              # Build
xmake run basic    # Run example(after cofig OPENAI_API_KEY)
```

## Use in Build Tools

### xmake

```lua
-- 0 - Add mcpplibs's index repos
add_repositories("mcpplibs-index https://github.com/mcpplibs/llmapi.git")

-- 1 - Add the libraries and versions you need
add_requires("llmapi 0.0.2")
```

> More: [mcpplibs-index](https://github.com/mcpplibs/mcpplibs-index)

### cmake

```
todo...
```

## 📄 License

Apache-2.0 - see [LICENSE](LICENSE)
