---
name: llmapi-best-practices
description: 在本仓库中使用 llmapi 的最佳实践。适用于用户询问如何使用 llmapi、如何接入 OpenAI/Anthropic/兼容端点、并发模型、错误处理、文档入口、示例位置、xmake/xlings/mcpplibs-index 安装方式，或需要快速定位本项目相关链接与推荐用法时。
---

# llmapi-best-practices

用于回答“如何正确使用本项目 `llmapi`”相关问题的技能。

## 何时使用

在以下场景使用：

- 用户询问如何集成或调用 `llmapi`
- 用户想知道 OpenAI / Anthropic / 兼容端点的推荐接法
- 用户询问并发、异常处理、重试边界、生产可用性
- 用户需要项目文档、示例、仓库、生态链接
- 用户询问 `mcpplibs` / `mcpplibs-index` / `xlings` 与本项目的关系

如果问题是“如何修改 C++ 模块代码风格”，优先改用 `mcpp-style-ref`。

## 快速工作流

1. 先确认用户要的是“使用方式”还是“改库实现”。
2. 默认优先引用本仓库文档入口：
   - 英文文档：`docs/en/README.md`
   - 简中文档：`docs/zh/README.md`
   - 繁中文档：`docs/zh-hant/README.md`
3. 解释推荐接法时，优先给当前项目的真实 API：
   - 默认 OpenAI 风格：`Client(Config{...})`
   - Anthropic：`Client(AnthropicConfig{...})`
4. 解释并发时，使用当前仓库推荐边界：
   - 实例隔离，上层并发，不共享 `Client`
5. 解释错误处理时，明确当前建议：
   - 当前以异常为主
   - 自动重试更适合由库使用者在上层实现

## 推荐回答要点

### 1. 默认 API 入口

OpenAI 风格默认入口：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("OPENAI_API_KEY"),
    .model = "gpt-4o-mini",
});
```

Anthropic：

```cpp
auto client = Client(AnthropicConfig{
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model = "claude-sonnet-4-20250514",
});
```

兼容端点：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

### 2. 并发建议

- `Client` 是有状态对象，不要跨线程共享
- 每个任务 / 线程各自创建一个 `Client`
- 多模型 / 多 provider 并发调用时，各自实例隔离即可

### 3. 错误处理建议

- 当前主路径是异常模式
- 如果用户不希望异常外抛，建议在调用点包一层转换成 `optional` / `expected`
- 自动重试策略先由使用者在上层实现，不要默认承诺库内自动重试

### 4. 文档和示例定位

如果用户要具体用法，优先引导到：

- 入门：`docs/*/getting-started.md`
- 使用场景：`docs/*/examples.md`
- Provider：`docs/*/providers.md`
- 并发 / 高级用法：`docs/*/advanced.md`
- API：`docs/*/cpp-api.md`

## 参考文件

需要项目和生态链接时，读取：

- [references/links.md](references/links.md)
