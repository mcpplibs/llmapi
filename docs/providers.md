# Providers Configuration

How to configure the providers that exist in the current codebase.

## URL Constants

`mcpplibs::llmapi::URL` exposes common base URLs:

```cpp
import mcpplibs.llmapi;
using namespace mcpplibs::llmapi;

URL::OpenAI
URL::Anthropic
URL::DeepSeek
URL::OpenRouter
URL::Poe
```

## OpenAI

Use `openai::OpenAI` for OpenAI chat, streaming, tool calls, and embeddings.

```cpp
auto client = Client(openai::OpenAI({
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
}));
```

- Get keys from [OpenAI Platform](https://platform.openai.com/api-keys)
- Set `OPENAI_API_KEY`

## Anthropic

Use `anthropic::Anthropic` for Anthropic chat and streaming.

```cpp
auto client = Client(anthropic::Anthropic({
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model = "claude-sonnet-4-20250514",
}));
```

- Get keys from [Anthropic Console](https://console.anthropic.com/)
- Set `ANTHROPIC_API_KEY`

## OpenAI-Compatible Endpoints

The OpenAI provider accepts a custom `baseUrl`, so DeepSeek, OpenRouter, Poe, local gateways, and self-hosted OpenAI-compatible services can all reuse `openai::OpenAI`.

```cpp
auto client = Client(openai::OpenAI({
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
}));
```

You can also use a literal endpoint:

```cpp
auto client = Client(openai::OpenAI({
    .apiKey = std::getenv("OPENROUTER_API_KEY"),
    .baseUrl = "https://openrouter.ai/api/v1",
    .model = "openai/gpt-4o-mini",
}));
```

## Custom Headers And Proxy

```cpp
auto provider = openai::OpenAI({
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .baseUrl = std::string(URL::OpenAI),
    .model = "gpt-4o-mini",
    .proxy = "http://127.0.0.1:7890",
    .customHeaders = {
        {"X-Trace-Id", "demo-request"},
    },
});
```

## Environment Variables

Typical setup:

```bash
export OPENAI_API_KEY="sk-..."
export ANTHROPIC_API_KEY="sk-ant-..."
export DEEPSEEK_API_KEY="..."
export OPENROUTER_API_KEY="..."
```

## Provider Capabilities

| Provider | Chat | Streaming | Embeddings | Notes |
|----------|------|-----------|------------|-------|
| `openai::OpenAI` | yes | yes | yes | Also works with compatible endpoints |
| `anthropic::Anthropic` | yes | yes | no | Anthropic Messages API |

## Troubleshooting

### Connection Issues

```cpp
try {
    auto resp = client.chat("test");
} catch (const std::runtime_error& e) {
    std::cerr << "Network error: " << e.what() << '\n';
}
```

### Authentication Errors

```cpp
auto key = std::getenv("OPENAI_API_KEY");
if (!key || std::string(key).empty()) {
    std::cerr << "OPENAI_API_KEY not set\n";
}
```

### Model Not Found

```cpp
auto openaiClient = Client(openai::OpenAI({
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
}));

auto anthropicClient = Client(anthropic::Anthropic({
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model = "claude-sonnet-4-20250514",
}));
```

## See Also

- [Getting Started](getting-started.md)
- [C++ API Reference](cpp-api.md)
- [Examples](examples.md)
