# C++ API 參考

## 匯出模組

- `mcpplibs.llmapi`
- `mcpplibs.llmapi:types`
- `mcpplibs.llmapi:url`
- `mcpplibs.llmapi:coro`
- `mcpplibs.llmapi:provider`
- `mcpplibs.llmapi:client`
- `mcpplibs.llmapi:openai`
- `mcpplibs.llmapi:anthropic`

## 關鍵別名

- `Config` -> `openai::Config`
- `AnthropicConfig` -> `anthropic::Config`
- `OpenAI` -> `openai::OpenAI`
- `Anthropic` -> `anthropic::Anthropic`

## Client 入口

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
```

```cpp
auto client = Client(AnthropicConfig{
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model = "claude-sonnet-4-20250514",
});
```

## 常用方法

```cpp
client.system(...)
client.user(...)
client.add_message(...)
client.clear()

client.chat(...)
client.chat_async(...)
client.chat_stream(...)
client.chat_stream_async(...)

client.embed(...)
client.save_conversation(...)
client.load_conversation(...)
client.conversation()
client.provider()
```

## ChatParams

`ChatParams` 支援：

- `temperature`
- `topP`
- `maxTokens`
- `stop`
- `tools`
- `toolChoice`
- `responseFormat`
- `extraJson`

## ChatResponse

常用欄位與輔助方法：

- `id`
- `model`
- `content`
- `stopReason`
- `usage`
- `text()`
- `tool_calls()`
