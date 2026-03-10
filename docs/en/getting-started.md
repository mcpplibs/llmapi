# Getting Started

## Prerequisites

- **C++ Compiler**: GCC 14+, Clang 18+, or MSVC 2022+ with C++23 support
- **Build System**: [xmake](https://xmake.io/) 3.0.0+
- **Dependencies**: `mbedtls` is resolved automatically by xmake

## Installation

### Using xmake Package Manager

Add to your `xmake.lua`:

```lua
add_repositories("mcpplibs-index git@github.com:mcpplibs/mcpplibs-index.git")
add_requires("llmapi 0.0.2")

target("myapp")
    set_kind("binary")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_files("src/*.cpp")
    add_packages("llmapi")
```

### Building from Source

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi

xmake
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## First Example

Create `main.cpp`:

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

    client.system("You are a helpful assistant.");
    auto resp = client.chat("Hello, introduce yourself in one sentence.");

    std::cout << resp.text() << '\n';
    return 0;
}
```

Build and run:

```bash
xmake
xmake run hello_mcpp
```

## Environment Setup

Set the provider-specific API key you plan to use:

```bash
export OPENAI_API_KEY="sk-..."
export ANTHROPIC_API_KEY="sk-ant-..."
export DEEPSEEK_API_KEY="..."
```

## Switching Providers

OpenAI:

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
```

Anthropic:

```cpp
auto client = Client(anthropic::Anthropic({
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model = "claude-sonnet-4-20250514",
}));
```

Compatible endpoint through the OpenAI provider:

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## Next Steps

- [C++ API Guide](cpp-api.md) - Learn the full C++ API
- [Examples](examples.md) - See more examples
- [Providers](providers.md) - Configure different providers
