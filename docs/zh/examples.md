# 示例

## 最小聊天

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});

client.system("You are a helpful assistant.");
auto resp = client.chat("请用一句话解释 C++23 模块。");
std::cout << resp.text() << '\n';
```

## 流式输出

```cpp
std::string collected;
client.chat_stream("写一首关于模板的三行短诗。", [&](std::string_view chunk) {
    collected += chunk;
    std::cout << chunk;
});
```

## 多轮对话

```cpp
auto resp1 = client.chat("法国首都是哪里？");
auto resp2 = client.chat("它大概有多少人口？");
```

## 保存与恢复对话

```cpp
client.chat("记住我最喜欢的语言是 C++。");
client.save_conversation("conversation.json");

auto restored = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
restored.load_conversation("conversation.json");
```

## 工具调用

```cpp
auto params = ChatParams{
    .tools = std::vector<ToolDef>{{
        .name = "get_temperature",
        .description = "Get the temperature for a city",
        .inputSchema = R"({"type":"object","properties":{"city":{"type":"string"}},"required":["city"]})",
    }},
    .toolChoice = ToolChoice::Auto,
};

auto resp = client.chat("东京现在多少度？", params);
```

## Embeddings

```cpp
auto embedding = client.embed(
    {"hello world", "modern c++"},
    "text-embedding-3-small"
);
```

## 异常模式

当前库以抛异常作为失败报告方式。对于希望让错误直接上传的调用链，这是最直接的使用方式。

```cpp
try {
    auto resp = client.chat("请用一段话解释 RAII。");
    std::cout << resp.text() << '\n';
} catch (const ApiError& e) {
    std::cerr << "API error: status=" << e.statusCode << " body=" << e.body << '\n';
} catch (const ConnectionError& e) {
    std::cerr << "Connection error: " << e.what() << '\n';
}
```

## 无异常模式

如果你的上层不希望异常继续外抛，可以在调用点自行包一层，转换成 `optional`、`expected` 或业务自己的结果类型。

```cpp
std::optional<std::string> safe_chat(std::string_view prompt) {
    try {
        return client.chat(prompt).text();
    } catch (...) {
        return std::nullopt;
    }
}
```

## 推荐的上层重试

当前更推荐由库使用者在上层实现重试，因为是否可重试取决于业务语义。

```cpp
for (int attempt = 0; attempt < 3; ++attempt) {
    try {
        return client.chat(prompt).text();
    } catch (const ConnectionError&) {
    } catch (const ApiError& e) {
        if (e.statusCode != 429 && (e.statusCode < 500 || e.statusCode >= 600)) {
            throw;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200 * (1 << attempt)));
}
```

## 并发使用

推荐模式是“实例隔离，上层并发”，也就是每个任务 / 线程各自创建一个 `Client`。

```cpp
auto futureA = std::async(std::launch::async, [] {
    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });
    return client.chat("总结一下 modules。").text();
});

auto futureB = std::async(std::launch::async, [] {
    auto client = Client(AnthropicConfig{
        .apiKey = std::getenv("ANTHROPIC_API_KEY"),
        .model = "claude-sonnet-4-20250514",
    });
    return client.chat("把 hello world 翻译成日语。").text();
});
```
