# llmapi

> Modern C++23 LLM client built with modules

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| English - [简体中文](README.zh.md) - [繁體中文](README.zh.hant.md) |
|:---:|
| [Documentation](docs/) - [C++ API](docs/cpp-api.md) - [Examples](docs/examples.md) |

`llmapi` provides a typed `Client<Provider>` API for chat, streaming, embeddings, tool calls, and conversation persistence. The repository ships built-in providers for OpenAI and Anthropic, and the OpenAI provider can target compatible endpoints through a custom `baseUrl`.

## Features

- `import mcpplibs.llmapi` with C++23 modules
- Strongly typed messages, tools, and response structs
- Sync, async, and streaming chat APIs
- Embeddings via the OpenAI provider
- Conversation save/load helpers
- OpenAI-compatible endpoint support through `openai::Config::baseUrl`

## Quick Start

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::cerr << "OPENAI_API_KEY not set\n";
        return 1;
    }

    auto client = Client(openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    }));

    client.system("You are a concise assistant.");
    auto resp = client.chat("Explain why C++23 modules are useful in two sentences.");

    std::cout << resp.text() << '\n';
    return 0;
}
```

## Providers

- `openai::OpenAI` for OpenAI chat, streaming, embeddings, and OpenAI-compatible endpoints
- `anthropic::Anthropic` for Anthropic chat and streaming

Compatible endpoints can reuse the OpenAI provider:

```cpp
auto provider = openai::OpenAI({
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## Build And Run

```bash
xmake
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## Package Usage

```lua
add_repositories("mcpplibs-index https://github.com/mcpplibs/mcpplibs-index.git")
add_requires("llmapi 0.0.2")

target("demo")
    set_kind("binary")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_files("src/*.cpp")
    add_packages("llmapi")
```

See [docs/getting-started.md](docs/getting-started.md) and [docs/providers.md](docs/providers.md) for more setup detail.

## License

Apache-2.0 - see [LICENSE](LICENSE)
