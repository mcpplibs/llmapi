# Examples

Practical examples using the current `Client<Provider>` API.

## Minimal Chat

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    client.system("You are a helpful assistant.");
    auto resp = client.chat("In one sentence, explain C++23 modules.");
    std::cout << resp.text() << '\n';
}
```

## Streaming Response

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    std::string streamed;
    client.chat_stream("Write a 3-line poem about templates.", [&](std::string_view chunk) {
        streamed += chunk;
        std::cout << chunk;
    });
    std::cout << "\n\nCollected " << streamed.size() << " bytes\n";
}
```

## Multi-Turn Conversation

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    client.system("Reply briefly.");

    auto resp1 = client.chat("What is the capital of France?");
    auto resp2 = client.chat("What is its population roughly?");

    std::cout << resp1.text() << '\n';
    std::cout << resp2.text() << '\n';
    std::cout << "Messages stored: " << client.conversation().size() << '\n';
}
```

## Save And Load Conversation

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    client.chat("Remember that my favorite language is C++.");
    client.save_conversation("conversation.json");

    auto restored = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });
    restored.load_conversation("conversation.json");

    auto resp = restored.chat("What language do I like?");
    std::cout << resp.text() << '\n';
}
```

## Tool Calling

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    auto params = ChatParams{
        .tools = std::vector<ToolDef>{{
            .name = "get_temperature",
            .description = "Get the temperature for a city",
            .inputSchema = R"({"type":"object","properties":{"city":{"type":"string"}},"required":["city"]})",
        }},
        .toolChoice = ToolChoice::Auto,
    };

    auto resp = client.chat("What's the temperature in Tokyo?", params);
    for (const auto& call : resp.tool_calls()) {
        std::cout << call.name << ": " << call.arguments << '\n';
    }
}
```

## Embeddings

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    auto embedding = client.embed(
        {"hello world", "modern c++"},
        "text-embedding-3-small"
    );

    std::cout << "vectors: " << embedding.embeddings.size() << '\n';
    std::cout << "dimension: " << embedding.embeddings[0].size() << '\n';
}
```

## Exception Mode

The library currently reports failures by throwing exceptions. This is the recommended direct style when you want errors to propagate naturally.

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    try {
        auto client = Client(Config{
            .apiKey = std::getenv("OPENAI_API_KEY"),
            .model = "gpt-4o-mini",
        });

        auto resp = client.chat("Explain RAII in one paragraph.");
        std::cout << resp.text() << '\n';
    } catch (const ApiError& e) {
        std::cerr << "API error: status=" << e.statusCode << " body=" << e.body << '\n';
        return 2;
    } catch (const ConnectionError& e) {
        std::cerr << "Connection error: " << e.what() << '\n';
        return 3;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << '\n';
        return 4;
    }
}
```

## No-Exception Style At Call Site

If your application prefers not to let exceptions escape, wrap the call and convert the result to `std::optional`, `std::expected`, or your own result type.

```cpp
import mcpplibs.llmapi;
import std;

std::optional<std::string> safe_chat(std::string_view prompt) {
    using namespace mcpplibs::llmapi;

    try {
        auto client = Client(Config{
            .apiKey = std::getenv("OPENAI_API_KEY"),
            .model = "gpt-4o-mini",
        });
        return client.chat(prompt).text();
    } catch (...) {
        return std::nullopt;
    }
}
```

## Recommended Retry At The Application Layer

Retry policy is currently best implemented by the library user because retryability depends on business semantics.

```cpp
import mcpplibs.llmapi;
import std;

std::string chat_with_retry(std::string_view prompt) {
    using namespace mcpplibs::llmapi;

    for (int attempt = 0; attempt < 3; ++attempt) {
        try {
            auto client = Client(Config{
                .apiKey = std::getenv("OPENAI_API_KEY"),
                .model = "gpt-4o-mini",
            });
            return client.chat(prompt).text();
        } catch (const ConnectionError&) {
        } catch (const ApiError& e) {
            if (e.statusCode != 429 && (e.statusCode < 500 || e.statusCode >= 600)) {
                throw;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200 * (1 << attempt)));
    }

    throw std::runtime_error("retry limit exceeded");
}
```

## Parallel Use With Isolated Clients

The recommended concurrency model is one client per task or thread.

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto futureA = std::async(std::launch::async, [] {
        auto client = Client(Config{
            .apiKey = std::getenv("OPENAI_API_KEY"),
            .model = "gpt-4o-mini",
        });
        return client.chat("Summarize modules.").text();
    });

    auto futureB = std::async(std::launch::async, [] {
        auto client = Client(AnthropicConfig{
            .apiKey = std::getenv("ANTHROPIC_API_KEY"),
            .model = "claude-sonnet-4-20250514",
        });
        return client.chat("Translate 'hello world' to Japanese.").text();
    });

    std::cout << futureA.get() << '\n';
    std::cout << futureB.get() << '\n';
}
```

## See Also

- [C++ API Reference](cpp-api.md)
- [Providers Configuration](providers.md)
