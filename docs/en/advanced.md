# Advanced Usage

Advanced patterns using the current API surface.

## Conversation Persistence

Each `chat()` and `chat_stream()` call appends the user message and the assistant response to the in-memory `Conversation`.

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});

client.system("You are helpful.");
client.chat("Remember that I prefer concise answers.");
client.save_conversation("session.json");

auto restored = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
restored.load_conversation("session.json");
```

## Manual Message Control

You can seed few-shot history or inject tool results directly:

```cpp
client.clear();
client.add_message(Message::system("Translate English to Chinese."));
client.add_message(Message::user("hello"));
client.add_message(Message::assistant("你好"));
auto resp = client.chat("goodbye");
```

## Streaming

Streaming returns a full `ChatResponse` after the callback finishes, so you can combine live output with post-processing.

```cpp
std::string collected;
auto resp = client.chat_stream("Tell a story about templates.", [&](std::string_view chunk) {
    collected += chunk;
    std::cout << chunk;
});

std::cout << "\nstop reason=" << static_cast<int>(resp.stopReason) << '\n';
```

## Async API

```cpp
auto task = client.chat_async("Explain coroutines briefly.");
auto resp = task.get();
std::cout << resp.text() << '\n';
```

## Concurrency Model

Use instance isolation for concurrency:

- `Client` is stateful and not thread-safe
- `tinyhttps::HttpClient` is also not thread-safe
- create one `Client` per task or per thread
- do not share a single `Client` across concurrent callers

This works well for calling multiple providers in parallel because each client owns its own provider, conversation, and transport state.

```cpp
auto futureA = std::async(std::launch::async, [&] {
    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });
    return client.chat("summarize this");
});

auto futureB = std::async(std::launch::async, [&] {
    auto client = Client(AnthropicConfig{
        .apiKey = std::getenv("ANTHROPIC_API_KEY"),
        .model = "claude-sonnet-4-20250514",
    });
    return client.chat("translate this");
});
```

## Tool Calling Loop

The provider surfaces requested tools via `ChatResponse::tool_calls()`. You then append a tool result and continue the conversation.

```cpp
auto params = ChatParams{
    .tools = std::vector<ToolDef>{{
        .name = "get_weather",
        .description = "Return weather for a city",
        .inputSchema = R"({"type":"object","properties":{"city":{"type":"string"}},"required":["city"]})",
    }},
    .toolChoice = ToolChoice::Auto,
};

auto first = client.chat("What's the weather in Tokyo?", params);
for (const auto& call : first.tool_calls()) {
    client.add_message(Message{
        .role = Role::Tool,
        .content = std::vector<ContentPart>{
            ToolResultContent{
                .toolUseId = call.id,
                .content = R"({"temperature":"22C","condition":"sunny"})",
            },
        },
    });
}

auto final = client.provider().chat(client.conversation().messages, params);
```

## Compatible Endpoints

```cpp
auto provider = openai::OpenAI({
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## See Also

- [C++ API Reference](cpp-api.md)
- [Examples](examples.md)
- [Providers](providers.md)
