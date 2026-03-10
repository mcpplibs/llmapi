# llmapi

> Modern C++23 LLM client built with modules

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| English - [简体中文](README.zh.md) - [繁體中文](README.zh.hant.md) |
|:---:|
| [Documentation](docs/README.md) - [English Docs](docs/en/README.md) - [中文文档](docs/zh/README.md) - [繁體中文文件](docs/zh-hant/README.md) |

`llmapi` provides a typed `Client<Provider>` API for chat, streaming, embeddings, tool calls, and conversation persistence. The default config alias `Config` maps to OpenAI-style providers, so the common case does not need an explicit `openai::OpenAI` wrapper.

## Features

- `import mcpplibs.llmapi` with C++23 modules
- Strongly typed messages, tools, and response structs
- Sync, async, and streaming chat APIs
- Embeddings via the OpenAI provider
- Conversation save/load helpers
- OpenAI-compatible endpoint support through `openai::Config::baseUrl`

## Production Readiness

`llmapi` is usable for internal tools, prototypes, and early production experiments, but it should not yet be treated as fully industrial-grade infrastructure.

Required gaps before that bar:

- Unified error model across providers and transport
- Retry, backoff, timeout, and idempotency policy
- Request cancellation for long-running and streaming calls
- Logging, metrics, trace hooks, and request correlation
- Hardening of the custom HTTP/TLS transport layer
- Fault-injection, concurrency, and large-scale mock testing
- Stronger API compatibility and versioning guarantees
- More complete production configuration surface
- Explicit thread-safety and concurrency semantics
- Operational documentation for retries, keys, proxies, and failure handling

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

    auto client = Client(Config{
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    });

    client.system("You are a concise assistant.");
    auto resp = client.chat("Explain why C++23 modules are useful in two sentences.");

    std::cout << resp.text() << '\n';
    return 0;
}
```

## Providers

- `openai::OpenAI` for OpenAI chat, streaming, embeddings, and OpenAI-compatible endpoints
- `anthropic::Anthropic` for Anthropic chat and streaming
- `Config` as a convenient alias for `openai::Config`

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

See [docs/en/getting-started.md](docs/en/getting-started.md), [docs/en/providers.md](docs/en/providers.md), and [docs/en/README.md](docs/en/README.md) for more setup and readiness detail.

## License

Apache-2.0 - see [LICENSE](LICENSE)
