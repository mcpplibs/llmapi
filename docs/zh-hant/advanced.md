# 進階用法

## 對話持久化

```cpp
client.chat("記住我偏好簡潔回答。");
client.save_conversation("session.json");
```

## 手動注入訊息

```cpp
client.add_message(Message::system("Translate English to Chinese."));
client.add_message(Message::user("hello"));
client.add_message(Message::assistant("你好"));
```

## 非同步介面

```cpp
auto task = client.chat_async("簡要解釋 coroutine。");
auto resp = task.get();
```

## 工具呼叫流程

```cpp
auto first = client.chat("東京天氣如何？", params);
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
