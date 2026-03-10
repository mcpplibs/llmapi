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

## See Also

- [C++ API Reference](cpp-api.md)
- [Providers Configuration](providers.md)
