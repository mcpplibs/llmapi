# Providers Configuration

How to use different LLM providers.

## Built-in Provider URLs

The library includes predefined constants for popular providers:

```cpp
// C++
import mcpplibs.llmapi;
using namespace mcpplibs::llmapi;

URL::OpenAI    // "https://api.openai.com/v1"
URL::Poe       // "https://api.poe.com/v1"
URL::DeepSeek  // Add more as needed
```

## OpenAI

Official OpenAI API.

**C++:**
```cpp
Client client(std::getenv("OPENAI_API_KEY"), URL::OpenAI);
client.model("gpt-4");  // or gpt-4-turbo, gpt-3.5-turbo, etc.
```


**Get API Key:**
- Visit [OpenAI Platform](https://platform.openai.com/api-keys)
- Create an API key
- Set environment variable: `export OPENAI_API_KEY="sk-..."`

## Poe

Poe platform (supports multiple models including GPT-4, Claude, etc.).

**C++:**
```cpp
Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
client.model("gpt-5");  // or claude-3-opus, etc.
```

**Get API Key:**
- Visit [Poe API](https://poe.com/api_key)
- Create an API key
- Set environment variable: `export OPENAI_API_KEY="your-poe-key"`

**Available Models:**
- `gpt-5` - Latest GPT model
- `claude-3-opus` - Anthropic Claude
- `gemini-pro` - Google Gemini
- And more...

## DeepSeek

DeepSeek AI platform.

**C++:**
```cpp
Client client(apiKey, URL::DeepSeek);
client.model("deepseek-chat");
```

## Custom Provider

Any OpenAI-compatible endpoint.

**C++:**
```cpp
Client client(apiKey, "https://your-endpoint.com/v1");
client.model("your-model");
```

## Azure OpenAI

**C++:**
```cpp
// Azure endpoint format: https://{resource}.openai.azure.com/openai/deployments/{deployment}
Client client(
    azureApiKey,
    "https://your-resource.openai.azure.com/openai/deployments/your-deployment"
);
```

## Local Models (Ollama, LM Studio, etc.)

Any local server with OpenAI-compatible API.

**C++:**
```cpp
// Ollama example
Client client("dummy-key", "http://localhost:11434/v1");
client.model("llama2");
```

## Environment Variables

Recommended approach for API keys:

```bash
# OpenAI
export OPENAI_API_KEY="sk-..."

# Poe
export OPENAI_API_KEY="your-poe-key"

# Azure
export AZURE_OPENAI_KEY="..."

# Custom
export CUSTOM_API_KEY="..."
```

Then in code:

**C++:**
```cpp
Client client(std::getenv("OPENAI_API_KEY"), URL::OpenAI);
```

## Provider Comparison

| Provider | Streaming | Models | Notes |
|----------|-----------|--------|-------|
| OpenAI | ✅ | GPT-4, GPT-3.5 | Official API |
| Poe | ✅ | GPT, Claude, Gemini | Multiple models |
| DeepSeek | ✅ | DeepSeek-Chat | Chinese AI |
| Azure | ✅ | GPT models | Enterprise |
| Ollama | ✅ | Local models | Free, local |

## Switching Providers

Easy to switch between providers:

```cpp
// Development - use Poe
Client dev_client(std::getenv("POE_KEY"), URL::Poe);
dev_client.model("gpt-5");

// Production - use OpenAI
Client prod_client(std::getenv("OPENAI_API_KEY"), URL::OpenAI);
prod_client.model("gpt-4");

// Local testing - use Ollama
Client test_client("dummy", "http://localhost:11434/v1");
test_client.model("llama2");
```

## Troubleshooting

### Connection Issues

```cpp
try {
    client.user("test").request();
} catch (const std::runtime_error& e) {
    std::println("Network error: {}", e.what());
    // Check: internet connection, firewall, proxy settings
}
```

### Authentication Errors

```cpp
// Check API key is set
auto key = std::getenv("OPENAI_API_KEY");
if (!key || std::string(key).empty()) {
    std::println("Error: API key not set");
}
```

### Model Not Found

```cpp
// Verify model name is correct for your provider
client.model("gpt-4");  // OpenAI
client.model("gpt-5");  // Poe
client.model("deepseek-chat");  // DeepSeek
```

## See Also

- [Getting Started](getting-started.md)
- [C++ API Reference](cpp-api.md)
- [Examples](examples.md)
