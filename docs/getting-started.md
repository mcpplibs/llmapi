# Getting Started

## Prerequisites

- **C++ Compiler**: GCC 14+, Clang 18+, or MSVC 2022+ with C++23 support
- **Build System**: [xmake](https://xmake.io/) 3.0.0+
- **Dependencies**: libcurl 8.11.0+ (automatically managed by xmake)

## Installation

### Using xmake Package Manager

Add to your `xmake.lua`:

```lua
add_repositories("mcpplibs-index git@github.com:mcpplibs/mcpplibs-index.git")
add_requires("llmapi 0.0.1")

target("myapp")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("llmapi")
```

### Building from Source

```bash
# Clone repository
git clone https://github.com/mcpplibs/openai.git
cd openai

# Build
xmake

# Run examples
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## First Example

Create `hello.cpp`:

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs;
    
    llmapi::Client client(std::getenv("OPENAI_API_KEY"), llmapi::URL::Poe);

    client.model("gpt-5")
          .system("You are a helpful assistant.")
          .user("Hello, introduce yourself in one sentence.")
          .request([](std::string_view chunk) {
              std::print("{}", chunk);
              std::cout.flush();
          });

    return 0;
}
```

Build and run:

```bash
xmake
xmake run hello
```

## Environment Setup

Set your API key:

```bash
# OpenAI
export OPENAI_API_KEY="sk-..."

# Poe
export OPENAI_API_KEY="your-poe-api-key"
```

## Next Steps

- [C++ API Guide](cpp-api.md) - Learn the full C++ API
- [C API Guide](c-api.md) - Learn the full C API
- [Examples](examples.md) - See more examples
- [Providers](providers.md) - Configure different LLM providers
