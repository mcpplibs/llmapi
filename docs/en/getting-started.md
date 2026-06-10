# Getting Started

## Prerequisites

- **Build Tool**: [mcpp](https://github.com/mcpp-community/mcpp) — toolchain and
  dependencies (e.g. `tinyhttps`) are resolved automatically
- Alternatively the library can be consumed from xmake — see
  [Using llmapi with xmake](xmake.md)

## Installation

### Start From a Template (recommended)

```bash
mcpp new myagent --template llmapi
cd myagent
mcpp run
```

List the templates the library ships:

```bash
mcpp new --list-templates llmapi
```

### Add To an Existing Project

```bash
mcpp add llmapi
```

Or declare it in `mcpp.toml`:

```toml
[dependencies.mcpplibs]
llmapi = "0.2.7"
```

### Building from Source

```bash
git clone https://github.com/mcpplibs/llmapi.git
cd llmapi
mcpp build
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
mcpp run
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
