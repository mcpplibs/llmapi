# 範例

## 最小聊天

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});

client.system("You are a helpful assistant.");
auto resp = client.chat("請用一句話解釋 C++23 模組。");
std::cout << resp.text() << '\n';
```

## 串流輸出

```cpp
std::string collected;
client.chat_stream("寫一首關於模板的三行短詩。", [&](std::string_view chunk) {
    collected += chunk;
    std::cout << chunk;
});
```

## 多輪對話

```cpp
auto resp1 = client.chat("法國首都是哪裡？");
auto resp2 = client.chat("它大概有多少人口？");
```

## 儲存與還原對話

```cpp
client.chat("記住我最喜歡的語言是 C++。");
client.save_conversation("conversation.json");

auto restored = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
restored.load_conversation("conversation.json");
```

## 工具呼叫

```cpp
auto params = ChatParams{
    .tools = std::vector<ToolDef>{{
        .name = "get_temperature",
        .description = "Get the temperature for a city",
        .inputSchema = R"({"type":"object","properties":{"city":{"type":"string"}},"required":["city"]})",
    }},
    .toolChoice = ToolChoice::Auto,
};

auto resp = client.chat("東京現在幾度？", params);
```

## Embeddings

```cpp
auto embedding = client.embed(
    {"hello world", "modern c++"},
    "text-embedding-3-small"
);
```
