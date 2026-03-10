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

## 並發模型

建議使用「實例隔離、上層並發」的方式：

- `Client` 是有狀態物件，不保證執行緒安全
- `tinyhttps::HttpClient` 也不保證執行緒安全
- 每個任務或執行緒各自建立一個 `Client`
- 不要把同一個 `Client` 共享給多個並發呼叫方

這種方式天然適合多模型 / 多 provider 並發呼叫，因為每個實例都持有獨立的 provider、對話與傳輸狀態。

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
