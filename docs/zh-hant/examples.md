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

## 例外模式

目前函式庫以拋出例外作為失敗回報方式。對於希望讓錯誤自然向上傳遞的呼叫鏈，這是最直接的使用方式。

```cpp
try {
    auto resp = client.chat("請用一段話解釋 RAII。");
    std::cout << resp.text() << '\n';
} catch (const ApiError& e) {
    std::cerr << "API error: status=" << e.statusCode << " body=" << e.body << '\n';
} catch (const ConnectionError& e) {
    std::cerr << "Connection error: " << e.what() << '\n';
}
```

## 無例外模式

如果上層不希望讓例外繼續外拋，可以在呼叫點自行包一層，轉成 `optional`、`expected` 或業務自己的結果型別。

```cpp
std::optional<std::string> safe_chat(std::string_view prompt) {
    try {
        return client.chat(prompt).text();
    } catch (...) {
        return std::nullopt;
    }
}
```

## 建議的上層重試

目前更建議由函式庫使用者在上層實作重試，因為是否可重試取決於業務語義。

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

## 並發使用

建議模式是「實例隔離、上層並發」，也就是每個任務 / 執行緒各自建立一個 `Client`。

```cpp
auto futureA = std::async(std::launch::async, [] {
    auto client = Client(Config{
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model = "gpt-4o-mini",
    });
    return client.chat("總結一下 modules。").text();
});

auto futureB = std::async(std::launch::async, [] {
    auto client = Client(AnthropicConfig{
        .apiKey = std::getenv("ANTHROPIC_API_KEY"),
        .model = "claude-sonnet-4-20250514",
    });
    return client.chat("把 hello world 翻譯成日語。").text();
});
```
