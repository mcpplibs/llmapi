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
