# llmapi

> Modern C++23 LLM client built with modules — `import` and chat, OpenAI-compatible, template-ready

[![Release](https://img.shields.io/github/v/release/mcpplibs/llmapi)](https://github.com/mcpplibs/llmapi/releases)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| English - [简体中文](README.zh.md) - [繁體中文](README.zh.hant.md) |
|:---:|
| [mcpp build tool](https://github.com/mcpp-community/mcpp) · [package index](https://github.com/mcpp-community/mcpp-index) · [Documentation](docs/README.md) · [Issues](https://github.com/mcpplibs/llmapi/issues) |

`llmapi` provides a typed `Client<Provider>` API for chat, streaming, embeddings, tool calls, and conversation persistence. The default config alias `Config` maps to OpenAI-style providers, so the common case does not need an explicit `openai::OpenAI` wrapper.

## Quick Start (mcpp)

```bash
mcpp new myagent --template llmapi && cd myagent
export OPENAI_API_KEY="sk-..."
mcpp run
```

Templates ship with the library and version-track it automatically:

```bash
mcpp new --list-templates llmapi              # list available templates
mcpp new mybot --template llmapi:chat        # interactive streaming chat CLI
mcpp new mybot --template llmapi:anthropic   # Anthropic provider
mcpp new mybot --template llmapi:deepseek    # OpenAI-compatible endpoint (DeepSeek)
```

Or add it to an existing mcpp project:

```bash
mcpp add llmapi
```

```toml
[dependencies.mcpplibs]
llmapi = "0.2.7"
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
    auto resp = client.chat("Explain why C++23 modules are useful in two sentences.");

    std::println("{}", resp.text());
    return 0;
}
```

## Features

- `import mcpplibs.llmapi` with C++23 modules
- Strongly typed messages, tools, and response structs
- Sync, async, and streaming chat APIs
- Embeddings via the OpenAI provider
- Conversation save/load helpers
- OpenAI-compatible endpoint support through `openai::Config::baseUrl`
- Project templates: `mcpp new <name> --template llmapi[:<template>]`

## Templates

| Template | Description |
|---|---|
| `openai` (default) | Minimal OpenAI chat — one request, one answer |
| `chat` | Interactive streaming chat CLI (OpenAI) |
| `anthropic` | Anthropic (Claude) chat |
| `deepseek` | OpenAI-compatible endpoint via `baseUrl` (DeepSeek) |

## Providers

- `openai::OpenAI` for OpenAI chat, streaming, embeddings, and OpenAI-compatible endpoints
- `anthropic::Anthropic` for Anthropic chat and streaming
- `Config` as a convenient alias for `openai::Config`

Compatible endpoints can reuse the OpenAI provider:

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## Build From Source

```bash
git clone https://github.com/mcpplibs/llmapi.git && cd llmapi
mcpp build
```

## Documentation

- [Getting Started](docs/en/getting-started.md)
- [Providers](docs/en/providers.md)
- [C++ API Guide](docs/en/cpp-api.md)
- [Using llmapi with xmake](docs/en/xmake.md)
- [Docs index](docs/README.md)

## License

Apache-2.0 - see [LICENSE](LICENSE)
