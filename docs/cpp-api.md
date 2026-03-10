# C++ API Reference

Reference for the current public module interface.

## Namespace

```cpp
import mcpplibs.llmapi;
using namespace mcpplibs::llmapi;
```

## Exported Modules

- `mcpplibs.llmapi`
- `mcpplibs.llmapi:types`
- `mcpplibs.llmapi:url`
- `mcpplibs.llmapi:coro`
- `mcpplibs.llmapi:provider`
- `mcpplibs.llmapi:client`
- `mcpplibs.llmapi:openai`
- `mcpplibs.llmapi:anthropic`
- `mcpplibs.llmapi:errors`

## Core Types

Important exported structs and enums:

- `Role`
- `Message`
- `TextContent`, `ImageContent`, `AudioContent`
- `ToolDef`, `ToolCall`, `ToolUseContent`, `ToolResultContent`
- `ChatParams`
- `ChatResponse`
- `EmbeddingResponse`
- `Conversation`
- `Usage`
- `ResponseFormat`

## Provider Concepts

```cpp
template<typename P>
concept Provider = requires(P p, const std::vector<Message>& messages, const ChatParams& params) {
    { p.name() } -> std::convertible_to<std::string_view>;
    { p.chat(messages, params) } -> std::same_as<ChatResponse>;
    { p.chat_async(messages, params) } -> std::same_as<Task<ChatResponse>>;
};
```

```cpp
template<typename P>
concept StreamableProvider = Provider<P> && requires(
    P p,
    const std::vector<Message>& messages,
    const ChatParams& params,
    std::function<void(std::string_view)> cb
) {
    { p.chat_stream(messages, params, cb) } -> std::same_as<ChatResponse>;
    { p.chat_stream_async(messages, params, cb) } -> std::same_as<Task<ChatResponse>>;
};
```

```cpp
template<typename P>
concept EmbeddableProvider = Provider<P> && requires(
    P p,
    const std::vector<std::string>& inputs,
    std::string_view model
) {
    { p.embed(inputs, model) } -> std::same_as<EmbeddingResponse>;
};
```

## `Client<P>`

`Client` is a class template that owns a provider instance and a `Conversation`.

```cpp
template<Provider P>
class Client;
```

### Construction

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
```

`Config` is an exported alias for `openai::Config`, so the default path uses an OpenAI-style provider without explicitly writing `openai::OpenAI`.

### Configuration

```cpp
Client& default_params(ChatParams params)
```

Stores default parameters used by `chat()`, `chat_async()`, and `chat_stream()`.

### Conversation Management

```cpp
Client& system(std::string_view content)
Client& user(std::string_view content)
Client& add_message(Message msg)
Client& clear()
```

### Synchronous Chat

```cpp
ChatResponse chat(std::string_view userMessage)
ChatResponse chat(std::string_view userMessage, ChatParams params)
```

`chat()` appends the user message, sends the full conversation, stores the assistant text response, and returns the parsed `ChatResponse`.

### Async Chat

```cpp
Task<ChatResponse> chat_async(std::string_view userMessage)
```

### Streaming Chat

```cpp
ChatResponse chat_stream(
    std::string_view userMessage,
    std::function<void(std::string_view)> callback
)
```

Available only when `P` satisfies `StreamableProvider`.

```cpp
Task<ChatResponse> chat_stream_async(
    std::string_view userMessage,
    std::function<void(std::string_view)> callback
)
```

### Embeddings

```cpp
EmbeddingResponse embed(const std::vector<std::string>& inputs, std::string_view model)
```

Available only when `P` satisfies `EmbeddableProvider`.

### Accessors

```cpp
const Conversation& conversation() const
Conversation& conversation()
void save_conversation(std::string_view filePath) const
void load_conversation(std::string_view filePath)
const P& provider() const
P& provider()
```

## Provider Config Types

```cpp
openai::Config {
    std::string apiKey;
    std::string baseUrl { "https://api.openai.com/v1" };
    std::string model;
    std::string organization;
    std::optional<std::string> proxy;
    std::map<std::string, std::string> customHeaders;
}
```

```cpp
anthropic::Config {
    std::string apiKey;
    std::string baseUrl { "https://api.anthropic.com/v1" };
    std::string model;
    std::string version { "2023-06-01" };
    int defaultMaxTokens { 4096 };
    std::optional<std::string> proxy;
    std::map<std::string, std::string> customHeaders;
}
```

## `ChatParams`

```cpp
struct ChatParams {
    std::optional<double> temperature;
    std::optional<double> topP;
    std::optional<int> maxTokens;
    std::optional<std::vector<std::string>> stop;
    std::optional<std::vector<ToolDef>> tools;
    std::optional<ToolChoicePolicy> toolChoice;
    std::optional<ResponseFormat> responseFormat;
    std::optional<std::string> extraJson;
};
```

## `ChatResponse`

```cpp
struct ChatResponse {
    std::string id;
    std::string model;
    std::vector<ContentPart> content;
    StopReason stopReason;
    Usage usage;

    std::string text() const;
    std::vector<ToolCall> tool_calls() const;
};
```

## `Conversation`

```cpp
struct Conversation {
    std::vector<Message> messages;

    void push(Message msg);
    void clear();
    int size() const;
    void save(std::string_view filePath) const;
    static Conversation load(std::string_view filePath);
};
```

## Complete Example

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });

    client.default_params(ChatParams{
        .temperature = 0.2,
    });

    client.system("Be concise.");

    auto resp1 = client.chat("What is C++?");
    std::cout << resp1.text() << '\n';

    auto resp2 = client.chat_stream("Give me one example of a C++23 feature.", [](std::string_view chunk) {
        std::cout << chunk;
    });
    std::cout << "\nmessages=" << client.conversation().size() << '\n';

    return 0;
}
```

## Error Handling

```cpp
try {
    auto resp = client.chat("Hello");
} catch (const std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << '\n';
}
```
