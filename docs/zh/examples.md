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
