# Provider 配置

## URL 常量

```cpp
URL::OpenAI
URL::Anthropic
URL::DeepSeek
URL::OpenRouter
URL::Poe
```

## OpenAI

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
```

## Anthropic

```cpp
auto client = Client(AnthropicConfig{
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model = "claude-sonnet-4-20250514",
});
```

## OpenAI 兼容端点

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## 能力对比

| Provider | 聊天 | 流式 | Embeddings |
|----------|------|------|------------|
| `openai::OpenAI` | 是 | 是 | 是 |
| `anthropic::Anthropic` | 是 | 是 | 否 |
