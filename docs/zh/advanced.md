# 高级用法

## 对话持久化

```cpp
client.chat("记住我偏好简洁回答。");
client.save_conversation("session.json");
```

## 手动注入消息

```cpp
client.add_message(Message::system("Translate English to Chinese."));
client.add_message(Message::user("hello"));
client.add_message(Message::assistant("你好"));
```

## 异步接口

```cpp
auto task = client.chat_async("简要解释 coroutine。");
auto resp = task.get();
```

## 并发模型

推荐使用“实例隔离，上层并发”的方式：

- `Client` 是有状态对象，不保证线程安全
- `tinyhttps::HttpClient` 也不保证线程安全
- 每个任务或线程单独创建一个 `Client`
- 不要把同一个 `Client` 共享给多个并发调用方

这种方式天然适合多个模型 / 多个 provider 并发调用，因为每个实例都持有独立的 provider、会话和传输状态。

## 工具调用循环

```cpp
auto first = client.chat("东京天气如何？", params);
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
```
