# llmapi v0.1.0 设计文档

> 日期：2026-03-10
> 状态：Draft
> 作者：Sunrisepeak / Claude Code

---

## 1. 概述

### 1.1 目标

将 `mcpplibs.llmapi` 从当前的 OpenAI-only 单文件客户端，升级为生产级多 Provider LLM API 库：

- **多 Provider 抽象** — 统一接口适配 OpenAI / Anthropic（及未来 Provider）
- **完整功能** — Tool Calling、多模态输入、结构化输出、Embeddings
- **自研 tinyhttps** — 替换 libcurl，仅依赖 mbedtls，支持 musl-gcc 全静态链接
- **C++20 协程** — 同步 + `co_await` 异步双模式
- **跨平台** — Linux / macOS / Windows

### 1.2 设计原则

| 原则 | 说明 |
|------|------|
| 分层抽象 | 核心统一接口 + Provider 特有扩展 + raw JSON escape hatch |
| 编译期多态 | C++20 concepts 约束 Provider，零虚表开销 |
| 最小依赖 | 仅 mbedtls 3.6.1 + nlohmann/json，无 libcurl |
| 静态链接友好 | musl-gcc 全静态可行，无 .so 运行时依赖 |
| 独立可拆分 | tinyhttps 模块完全独立，未来可作为单独库发布 |
| mcpp 风格 | 遵循 mcpp-style-ref 命名与模块规范 |

### 1.3 决策记录

| 决策点 | 选择 | 理由 |
|--------|------|------|
| 抽象哲学 | 分层抽象 | 兼顾统一性和 Provider 特有能力 |
| 多态机制 | C++20 concepts | 零开销，符合项目 C++23 风格 |
| 异步策略 | C++20 协程 | 现代异步范式，自研最小封装 |
| Event Loop | `poll()` / `WSAPoll()` | 跨平台，无平台特定依赖 |
| TLS | mbedtls 3.6.1 | 静态链接友好，Apache 2.0 许可 |
| HTTP | 自研 tinyhttps | 仅实现 LLM API 所需子集，接口通用 |
| CA 证书 | 内嵌 + 系统 fallback | 独立部署自带证书，有系统证书时自动更新 |

---

## 2. 整体架构

### 2.1 分层总览

```
┌─────────────────────────────────────────────────────┐
│                  用户代码 (Application)               │
│   client.chat("hello") | co_await client.chat(...)  │
└──────────────────────┬──────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────┐
│              mcpplibs.llmapi  (核心层)                │
│                                                      │
│  Client<P>          统一泛型客户端                     │
│  concept Provider   Provider 约束                    │
│  ChatParams         通用请求参数                      │
│  Message/Content    统一消息模型                      │
│  ToolDef/ToolCall   工具调用抽象                      │
│  Task<T>            协程 return type                 │
└──────────┬─────────────────────┬────────────────────┘
           │                     │
┌──────────▼──────┐   ┌─────────▼─────────┐
│  :openai        │   │  :anthropic       │
│  Provider 实现   │   │  Provider 实现    │
│  - 请求构建      │   │  - 请求构建       │
│  - 响应解析      │   │  - 响应解析       │
│  - SSE 解析      │   │  - SSE 解析       │
│  - Auth header   │   │  - Auth header    │
└──────────┬──────┘   └─────────┬─────────┘
           │                     │
┌──────────▼─────────────────────▼────────────────────┐
│          mcpplibs.tinyhttps  (独立通用模块)            │
│                                                      │
│  HttpClient        连接管理 + keep-alive             │
│  HttpRequest       请求构建                          │
│  HttpResponse      响应解析 + chunked decoding       │
│  SseParser         SSE 事件流解析                    │
│  TlsSocket         mbedtls TLS 封装                  │
│  Socket            平台抽象 (BSD/Winsock)            │
│  CaBundle          内嵌 CA + 系统 fallback            │
└─────────────────────────────────────────────────────┘
           │
     ┌─────▼─────┐
     │  mbedtls   │  add_requires("mbedtls 3.6.1")
     │  3.6.1     │
     └───────────┘
```

### 2.2 文件结构

```
src/
├── llmapi.cppm                    # export module mcpplibs.llmapi
├── types.cppm                     # :types     — Message, Content, ToolDef, ChatParams...
├── provider.cppm                  # :provider  — concept Provider
├── client.cppm                    # :client    — Client<P> 泛型客户端
├── coro.cppm                      # :coro      — Task<T>, EventLoop
├── openai.cppm                    # :openai    — OpenAI Provider 实现
├── anthropic.cppm                 # :anthropic — Anthropic Provider 实现
├── json/
│   ├── json.cppm
│   └── json.hpp
└── tinyhttps/
    ├── tinyhttps.cppm             # export module mcpplibs.tinyhttps
    ├── socket.cppm                # :socket    — 平台 socket 抽象
    ├── tls.cppm                   # :tls       — mbedtls TLS 封装
    ├── http.cppm                  # :http      — HTTP/1.1 client
    ├── sse.cppm                   # :sse       — SSE parser
    ├── proxy.cppm                 # :proxy     — HTTP CONNECT 代理
    └── ca_bundle.cppm             # :ca_bundle — 内嵌 CA 证书
```

### 2.3 模块依赖关系

```
mcpplibs.llmapi
  ├── :client    → :provider, :types, :coro
  ├── :openai    → :types, mcpplibs.tinyhttps, nlohmann.json
  ├── :anthropic → :types, mcpplibs.tinyhttps, nlohmann.json
  ├── :types     → (独立)
  ├── :provider  → :types, :coro
  └── :coro      → (独立)

mcpplibs.tinyhttps  (完全独立，不依赖 llmapi)
  ├── :http      → :tls, :sse, :proxy
  ├── :tls       → :socket, :ca_bundle, mbedtls
  ├── :socket    → (平台 API)
  ├── :sse       → (独立)
  ├── :proxy     → :socket, :tls
  └── :ca_bundle → mbedtls
```

### 2.4 xmake 构建

```lua
-- tinyhttps (独立 target，未来可拆出去)
target("tinyhttps")
    set_kind("static")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_requires("mbedtls 3.6.1")
    add_files("src/tinyhttps/*.cppm")
    add_packages("mbedtls")

-- llmapi (依赖 tinyhttps)
target("llmapi")
    set_kind("static")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_deps("tinyhttps")
    add_files("src/*.cppm", "src/json/json.cppm")
```

---

## 3. 核心类型系统

### 3.1 消息模型

统一两家 API 的消息表示差异。关键矛盾：
- OpenAI: system 是 messages 数组中的一个 role
- Anthropic: system 是顶层独立字段，messages 只有 user/assistant

设计：统一用 `Role::System` 存入消息列表，由 Provider 在序列化时提取。

```cpp
// src/types.cppm
export module mcpplibs.llmapi:types;

import std;

export namespace mcpplibs::llmapi {

// ─── 角色 ───
enum class Role {
    System,
    User,
    Assistant,
    Tool        // OpenAI 的 tool 角色, Anthropic 用 tool_result content block
};

// ─── 内容块（统一多模态） ───
struct TextContent {
    std::string text;
};

struct ImageContent {
    std::string data;           // base64 编码或 URL
    std::string mediaType;      // "image/png", "image/jpeg", ...
    bool isUrl { false };       // true=URL, false=base64
};

struct AudioContent {
    std::string data;           // base64 编码
    std::string format;         // "wav", "mp3"
};

struct ToolUseContent {
    std::string id;
    std::string name;
    std::string inputJson;      // JSON string
};

struct ToolResultContent {
    std::string toolUseId;
    std::string content;        // 结果文本
    bool isError { false };
};

using ContentPart = std::variant<
    TextContent,
    ImageContent,
    AudioContent,
    ToolUseContent,
    ToolResultContent
>;

// 内容：纯文本(string) 或 多部分(vector<ContentPart>)
using Content = std::variant<
    std::string,
    std::vector<ContentPart>
>;

// ─── 消息 ───
struct Message {
    Role role;
    Content content;
    std::string name;           // 可选，参与者名称

    // 便捷构造
    static Message system(std::string_view text) {
        return { Role::System, std::string(text), {} };
    }
    static Message user(std::string_view text) {
        return { Role::User, std::string(text), {} };
    }
    static Message assistant(std::string_view text) {
        return { Role::Assistant, std::string(text), {} };
    }
};

// ─── 工具定义 ───
struct ToolDef {
    std::string name;           // 工具名称
    std::string description;    // 描述
    std::string inputSchema;    // JSON Schema string
};

// ─── 工具调用（响应中） ───
struct ToolCall {
    std::string id;             // 调用 ID
    std::string name;           // 工具名称
    std::string arguments;      // JSON string
};

// ─── 工具选择策略 ───
enum class ToolChoice {
    Auto,       // 模型决定
    None,       // 禁止调用
    Required,   // 必须调用（OpenAI: "required", Anthropic: "any"）
};

struct ToolChoiceForced {
    std::string name;   // 指定工具名
};

using ToolChoicePolicy = std::variant<ToolChoice, ToolChoiceForced>;

// ─── 结构化输出 ───
enum class ResponseFormatType {
    Text,           // 默认纯文本
    JsonObject,     // JSON 模式
    JsonSchema,     // JSON Schema 约束 (OpenAI only, Anthropic 降级为提示词)
};

struct ResponseFormat {
    ResponseFormatType type { ResponseFormatType::Text };
    std::string schemaName;     // json_schema 模式下的 schema name
    std::string schema;         // JSON Schema string
};

// ─── 请求参数 ───
struct ChatParams {
    // 通用参数（两家都支持）
    std::optional<double> temperature;
    std::optional<double> topP;
    std::optional<int> maxTokens;
    std::optional<std::vector<std::string>> stop;

    // 工具
    std::optional<std::vector<ToolDef>> tools;
    std::optional<ToolChoicePolicy> toolChoice;

    // 结构化输出
    std::optional<ResponseFormat> responseFormat;

    // Provider 特有参数 (escape hatch)
    std::optional<std::string> extraJson;   // 合并到请求 JSON 中
};

// ─── 停止原因 ───
enum class StopReason {
    EndOfTurn,      // 正常结束
    MaxTokens,      // 达到 token 上限
    ToolUse,        // 需要调用工具
    ContentFilter,  // 内容过滤 (OpenAI only)
    StopSequence,   // 命中停止序列 (Anthropic only)
};

// ─── Token 用量 ───
struct Usage {
    int inputTokens { 0 };
    int outputTokens { 0 };
    int totalTokens { 0 };      // inputTokens + outputTokens
};

// ─── 聊天响应 ───
struct ChatResponse {
    std::string id;
    std::string model;
    std::vector<ContentPart> content;   // 响应内容块
    StopReason stopReason;
    Usage usage;

    // 便捷方法：提取纯文本
    std::string text() const {
        std::string result;
        for (const auto& part : content) {
            if (auto* t = std::get_if<TextContent>(&part)) {
                result += t->text;
            }
        }
        return result;
    }

    // 便捷方法：提取工具调用
    std::vector<ToolCall> tool_calls() const {
        std::vector<ToolCall> calls;
        for (const auto& part : content) {
            if (auto* t = std::get_if<ToolUseContent>(&part)) {
                calls.push_back({ t->id, t->name, t->inputJson });
            }
        }
        return calls;
    }
};

// ─── Embedding 响应 ───
struct EmbeddingResponse {
    std::vector<std::vector<float>> embeddings;
    std::string model;
    Usage usage;
};

// ─── 对话序列化 ───
struct Conversation {
    std::vector<Message> messages;

    void save(std::string_view filePath) const;
    static Conversation load(std::string_view filePath);

    void push(Message msg) { messages.push_back(std::move(msg)); }
    void clear() { messages.clear(); }
    int size() const { return static_cast<int>(messages.size()); }
};

} // namespace mcpplibs::llmapi
```

### 3.2 Provider Concept

```cpp
// src/provider.cppm
export module mcpplibs.llmapi:provider;

import :types;
import :coro;

import std;

export namespace mcpplibs::llmapi {

// ─── 流式回调 concept ───
template<typename F>
concept StreamCallback = std::invocable<F, std::string_view> &&
                         std::same_as<std::invoke_result_t<F, std::string_view>, void>;

// ─── Provider concept ───
template<typename P>
concept Provider = requires(P p,
                            const std::vector<Message>& messages,
                            const ChatParams& params) {
    // 基本信息
    { p.name() } -> std::convertible_to<std::string_view>;

    // 同步请求
    { p.chat(messages, params) } -> std::same_as<ChatResponse>;

    // 异步请求（协程）
    { p.chat_async(messages, params) } -> std::same_as<Task<ChatResponse>>;
};

// ─── 可选能力 concepts（Provider 可选实现） ───

// 支持流式
template<typename P>
concept StreamableProvider = Provider<P> && requires(P p,
                            const std::vector<Message>& messages,
                            const ChatParams& params,
                            std::function<void(std::string_view)> cb) {
    { p.chat_stream(messages, params, cb) } -> std::same_as<ChatResponse>;
    { p.chat_stream_async(messages, params, cb) } -> std::same_as<Task<ChatResponse>>;
};

// 支持 Embeddings
template<typename P>
concept EmbeddableProvider = Provider<P> && requires(P p,
                            const std::vector<std::string>& inputs,
                            std::string_view model) {
    { p.embed(inputs, model) } -> std::same_as<EmbeddingResponse>;
};

} // namespace mcpplibs::llmapi
```

### 3.3 Client 泛型客户端

```cpp
// src/client.cppm
export module mcpplibs.llmapi:client;

import :types;
import :provider;
import :coro;

import std;

export namespace mcpplibs::llmapi {

template<Provider P>
class Client {
private:
    P provider_;
    Conversation conversation_;
    ChatParams defaultParams_;

public:
    // ─── 构造 ───
    explicit Client(P provider)
        : provider_(std::move(provider)) {}

    // ─── 配置（链式） ───
    Client& default_params(ChatParams params) {
        defaultParams_ = std::move(params);
        return *this;
    }

    // ─── 消息管理 ───
    Client& system(std::string_view content) {
        conversation_.push(Message::system(content));
        return *this;
    }

    Client& user(std::string_view content) {
        conversation_.push(Message::user(content));
        return *this;
    }

    Client& add_message(Message msg) {
        conversation_.push(std::move(msg));
        return *this;
    }

    Client& clear() {
        conversation_.clear();
        return *this;
    }

    // ─── 同步聊天 ───
    ChatResponse chat(std::string_view userMessage) {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat(conversation_.messages, defaultParams_);
        conversation_.push(Message::assistant(response.text()));
        return response;
    }

    ChatResponse chat(std::string_view userMessage, ChatParams params) {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat(conversation_.messages, params);
        conversation_.push(Message::assistant(response.text()));
        return response;
    }

    // ─── 异步聊天 ───
    Task<ChatResponse> chat_async(std::string_view userMessage) {
        conversation_.push(Message::user(userMessage));
        auto response = co_await provider_.chat_async(
            conversation_.messages, defaultParams_);
        conversation_.push(Message::assistant(response.text()));
        co_return response;
    }

    // ─── 流式聊天（需要 StreamableProvider） ───
    ChatResponse chat_stream(std::string_view userMessage,
                             StreamCallback auto&& callback)
        requires StreamableProvider<P>
    {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat_stream(
            conversation_.messages, defaultParams_,
            std::forward<decltype(callback)>(callback));
        conversation_.push(Message::assistant(response.text()));
        return response;
    }

    Task<ChatResponse> chat_stream_async(std::string_view userMessage,
                                          StreamCallback auto&& callback)
        requires StreamableProvider<P>
    {
        conversation_.push(Message::user(userMessage));
        auto response = co_await provider_.chat_stream_async(
            conversation_.messages, defaultParams_,
            std::forward<decltype(callback)>(callback));
        conversation_.push(Message::assistant(response.text()));
        co_return response;
    }

    // ─── Embeddings（需要 EmbeddableProvider） ───
    EmbeddingResponse embed(const std::vector<std::string>& inputs,
                            std::string_view model)
        requires EmbeddableProvider<P>
    {
        return provider_.embed(inputs, model);
    }

    // ─── 对话管理 ───
    const Conversation& conversation() const { return conversation_; }
    Conversation& conversation() { return conversation_; }

    void save_conversation(std::string_view filePath) const {
        conversation_.save(filePath);
    }

    void load_conversation(std::string_view filePath) {
        conversation_ = Conversation::load(filePath);
    }

    // ─── Provider 访问 ───
    const P& provider() const { return provider_; }
    P& provider() { return provider_; }
};

} // namespace mcpplibs::llmapi
```

---

## 4. Provider 实现

### 4.1 OpenAI Provider

```cpp
// src/openai.cppm
export module mcpplibs.llmapi:openai;

import :types;
import :coro;

import mcpplibs.tinyhttps;
import mcpplibs.llmapi.nlohmann.json;
import std;

export namespace mcpplibs::llmapi::openai {

using Json = nlohmann::json;

struct Config {
    std::string apiKey;
    std::string baseUrl { "https://api.openai.com/v1" };
    std::string model;
    std::string organization;       // 可选 OpenAI-Organization header

    // HTTP 配置
    std::optional<std::string> proxy;
    std::map<std::string, std::string> customHeaders;
};

class OpenAI {
private:
    Config config_;
    tinyhttps::HttpClient http_;

public:
    explicit OpenAI(Config config)
        : config_(std::move(config))
        , http_(tinyhttps::HttpClientConfig {
            .proxy = config_.proxy,
          })
    {}

    // ─── Provider concept 实现 ───

    std::string_view name() const { return "openai"; }

    ChatResponse chat(const std::vector<Message>& messages,
                      const ChatParams& params);

    Task<ChatResponse> chat_async(const std::vector<Message>& messages,
                                  const ChatParams& params);

    // ─── StreamableProvider ───

    ChatResponse chat_stream(const std::vector<Message>& messages,
                             const ChatParams& params,
                             std::function<void(std::string_view)> callback);

    Task<ChatResponse> chat_stream_async(const std::vector<Message>& messages,
                                         const ChatParams& params,
                                         std::function<void(std::string_view)> callback);

    // ─── EmbeddableProvider ───

    EmbeddingResponse embed(const std::vector<std::string>& inputs,
                            std::string_view model);

private:
    // ─── 序列化 ───
    Json serialize_messages_(const std::vector<Message>& messages) const;
    Json serialize_params_(const ChatParams& params, bool stream) const;
    Json serialize_tools_(const std::vector<ToolDef>& tools) const;

    // ─── 反序列化 ───
    ChatResponse parse_response_(const Json& json) const;
    void parse_sse_chunk_(const std::string& data,
                          std::string& fullResponse,
                          std::function<void(std::string_view)>& callback) const;

    // ─── HTTP ───
    tinyhttps::HttpRequest build_request_(const std::string& endpoint,
                                          const Json& payload) const;
};

} // namespace mcpplibs::llmapi::openai
```

**OpenAI 序列化要点：**

| 字段 | 处理方式 |
|------|----------|
| system message | 保留在 messages 数组中，`role: "system"` |
| content | 纯文本 → `string`；多模态 → `ContentPart[]` |
| tool_calls | 响应中 `message.tool_calls[]`，独立字段 |
| tool result | `role: "tool"` + `tool_call_id` |
| auth | `Authorization: Bearer {key}` |
| response_format | 直接映射 `json_object` / `json_schema` |
| max_tokens | 映射到 `max_completion_tokens` |

### 4.2 Anthropic Provider

```cpp
// src/anthropic.cppm
export module mcpplibs.llmapi:anthropic;

import :types;
import :coro;

import mcpplibs.tinyhttps;
import mcpplibs.llmapi.nlohmann.json;
import std;

export namespace mcpplibs::llmapi::anthropic {

using Json = nlohmann::json;

struct Config {
    std::string apiKey;
    std::string baseUrl { "https://api.anthropic.com/v1" };
    std::string model;
    std::string version { "2023-06-01" };   // anthropic-version header

    int defaultMaxTokens { 4096 };          // Anthropic 要求 max_tokens 必填

    // HTTP 配置
    std::optional<std::string> proxy;
    std::map<std::string, std::string> customHeaders;
};

class Anthropic {
private:
    Config config_;
    tinyhttps::HttpClient http_;

public:
    explicit Anthropic(Config config)
        : config_(std::move(config))
        , http_(tinyhttps::HttpClientConfig {
            .proxy = config_.proxy,
          })
    {}

    // ─── Provider concept 实现 ───

    std::string_view name() const { return "anthropic"; }

    ChatResponse chat(const std::vector<Message>& messages,
                      const ChatParams& params);

    Task<ChatResponse> chat_async(const std::vector<Message>& messages,
                                  const ChatParams& params);

    // ─── StreamableProvider ───

    ChatResponse chat_stream(const std::vector<Message>& messages,
                             const ChatParams& params,
                             std::function<void(std::string_view)> callback);

    Task<ChatResponse> chat_stream_async(const std::vector<Message>& messages,
                                         const ChatParams& params,
                                         std::function<void(std::string_view)> callback);

    // 注意：Anthropic 没有 Embeddings API，不实现 EmbeddableProvider

private:
    // ─── 序列化（关键差异点） ───

    // 从消息列表中提取 system 消息，放入顶层 system 字段
    std::pair<std::optional<std::string>, std::vector<Message>>
        extract_system_(const std::vector<Message>& messages) const;

    Json serialize_messages_(const std::vector<Message>& messages) const;
    Json serialize_params_(const ChatParams& params, bool stream) const;
    Json serialize_tools_(const std::vector<ToolDef>& tools) const;

    // ─── 反序列化 ───
    ChatResponse parse_response_(const Json& json) const;

    // Anthropic SSE 有 event type 行 (event: xxx)，需单独解析
    void parse_sse_event_(const std::string& eventType,
                          const std::string& data,
                          std::string& fullResponse,
                          std::function<void(std::string_view)>& callback) const;

    // ─── HTTP ───
    tinyhttps::HttpRequest build_request_(const std::string& endpoint,
                                          const Json& payload) const;
};

} // namespace mcpplibs::llmapi::anthropic
```

**Anthropic 序列化要点：**

| 字段 | 处理方式 |
|------|----------|
| system message | 从 messages 提取，放入顶层 `"system"` 字段 |
| content | 纯文本 → `string`；多模态 → `ContentBlock[]`（type: text/image） |
| image | `source.type: "base64"` + `media_type` 或 `source.type: "url"` |
| tool_use | 响应 content 中的 `type: "tool_use"` block |
| tool_result | user 消息 content 中的 `type: "tool_result"` block |
| auth | `x-api-key: {key}` + `anthropic-version: {version}` |
| max_tokens | **必填**，用 `defaultMaxTokens` 兜底 |
| response_format | 不支持 json_schema，降级为工具调用或提示词引导 |
| SSE | `event: content_block_delta` + `data: {...}` 格式，非 OpenAI 的 `data:` only |

### 4.3 两家 API 差异映射表

| 统一类型 | OpenAI 映射 | Anthropic 映射 |
|----------|-------------|----------------|
| `Role::System` | `messages[]: role="system"` | 顶层 `system` 字段 |
| `Role::Tool` | `role="tool"` + `tool_call_id` | user 消息中 `tool_result` block |
| `StopReason::EndOfTurn` | `finish_reason="stop"` | `stop_reason="end_turn"` |
| `StopReason::MaxTokens` | `finish_reason="length"` | `stop_reason="max_tokens"` |
| `StopReason::ToolUse` | `finish_reason="tool_calls"` | `stop_reason="tool_use"` |
| `Usage.inputTokens` | `usage.prompt_tokens` | `usage.input_tokens` |
| `Usage.outputTokens` | `usage.completion_tokens` | `usage.output_tokens` |
| `ImageContent` | `image_url.url` (URL/data URI) | `source.type` + `media_type` + `data` |
| `ToolDef.inputSchema` | `function.parameters` (JSON Schema) | `input_schema` (JSON Schema) |
| `ToolCall` | `tool_calls[].function` | content block `type="tool_use"` |
| `StreamCallback` | `data: {json}` → `choices[0].delta.content` | `event: content_block_delta` → `delta.text` |
| Stream 结束 | `data: [DONE]` | `event: message_stop` |

---

## 5. tinyhttps 模块设计

### 5.1 目标

通用 HTTPS 客户端模块，完全独立于 llmapi，未来可作为 `mcpplibs.tinyhttps` 单独发布。

### 5.2 核心接口

```cpp
// src/tinyhttps/tinyhttps.cppm
export module mcpplibs.tinyhttps;

export import :socket;
export import :tls;
export import :http;
export import :sse;
export import :proxy;
export import :ca_bundle;

import std;

export namespace mcpplibs::tinyhttps {

// ─── HTTP 方法 ───
enum class Method {
    GET, POST, PUT, DELETE_, PATCH, HEAD
};

// ─── HTTP 请求 ───
struct HttpRequest {
    Method method { Method::GET };
    std::string url;                        // 完整 URL: https://host/path
    std::map<std::string, std::string> headers;
    std::string body;

    // 便捷构造
    static HttpRequest post(std::string_view url, std::string_view body) {
        return { Method::POST, std::string(url),
                 {{"Content-Type", "application/json"}},
                 std::string(body) };
    }
};

// ─── HTTP 响应 ───
struct HttpResponse {
    int statusCode { 0 };
    std::string statusText;
    std::map<std::string, std::string> headers;
    std::string body;

    bool ok() const { return statusCode >= 200 && statusCode < 300; }
};

// ─── SSE 事件 ───
struct SseEvent {
    std::string event;      // event type (默认 "message")
    std::string data;       // event data
    std::string id;         // event id (可选)
};

// ─── SSE 回调 ───
template<typename F>
concept SseCallback = std::invocable<F, const SseEvent&> &&
                      std::same_as<std::invoke_result_t<F, const SseEvent&>, bool>;
                      // 返回 false 停止接收

// ─── 客户端配置 ───
struct HttpClientConfig {
    std::optional<std::string> proxy;       // HTTP 代理 URL
    int connectTimeoutMs { 10000 };         // 连接超时
    int readTimeoutMs { 60000 };            // 读超时
    bool verifySsl { true };                // TLS 证书验证
    bool keepAlive { true };                // 连接复用
};

// ─── HTTP 客户端 ───
class HttpClient {
private:
    HttpClientConfig config_;
    // 连接池（host:port → TlsSocket）
    std::map<std::string, TlsSocket> pool_;

public:
    explicit HttpClient(HttpClientConfig config = {});
    ~HttpClient();

    // 同步请求
    HttpResponse send(const HttpRequest& request);

    // 流式请求（SSE）
    HttpResponse send_stream(const HttpRequest& request,
                             SseCallback auto&& callback);

    // 配置
    HttpClientConfig& config() { return config_; }

private:
    TlsSocket& get_connection_(std::string_view host, int port);
    void return_connection_(std::string_view host, int port, TlsSocket socket);
};

} // namespace mcpplibs::tinyhttps
```

### 5.3 平台 Socket 抽象

```cpp
// src/tinyhttps/socket.cppm
export module mcpplibs.tinyhttps:socket;

import std;

export namespace mcpplibs::tinyhttps {

class Socket {
private:
#ifdef _WIN32
    using SocketHandle = unsigned long long;     // SOCKET
    static constexpr SocketHandle INVALID { ~0ULL };
#else
    using SocketHandle = int;
    static constexpr SocketHandle INVALID { -1 };
#endif
    SocketHandle handle_ { INVALID };

public:
    Socket() = default;
    ~Socket();

    // Move only (RAII)
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 连接
    bool connect(std::string_view host, int port, int timeoutMs);
    void close();
    bool is_valid() const { return handle_ != INVALID; }

    // 读写
    int read(void* buf, int len);
    int write(const void* buf, int len);

    // 等待可读/可写 (poll-based)
    bool wait_readable(int timeoutMs);
    bool wait_writable(int timeoutMs);

    // 底层 handle（供 TLS 层使用）
    SocketHandle handle() const { return handle_; }

    // 平台初始化 (Windows: WSAStartup)
    static void platform_init();
    static void platform_cleanup();
};

} // namespace mcpplibs::tinyhttps
```

### 5.4 TLS 封装

```cpp
// src/tinyhttps/tls.cppm
export module mcpplibs.tinyhttps:tls;

import :socket;
import :ca_bundle;
import std;

export namespace mcpplibs::tinyhttps {

class TlsSocket {
private:
    Socket socket_;
    // mbedtls 内部状态 (pimpl 或直接持有)
    struct TlsState;
    std::unique_ptr<TlsState> state_;

public:
    TlsSocket() = default;
    ~TlsSocket();

    // Move only
    TlsSocket(TlsSocket&&) noexcept;
    TlsSocket& operator=(TlsSocket&&) noexcept;

    // 连接 + TLS 握手
    bool connect(std::string_view host, int port,
                 int timeoutMs, bool verifySsl);
    void close();
    bool is_valid() const;

    // 加密读写
    int read(void* buf, int len);
    int write(const void* buf, int len);

    // poll (包装底层 socket)
    bool wait_readable(int timeoutMs);
};

} // namespace mcpplibs::tinyhttps
```

### 5.5 SSE Parser

```cpp
// src/tinyhttps/sse.cppm
export module mcpplibs.tinyhttps:sse;

import std;

export namespace mcpplibs::tinyhttps {

struct SseEvent;  // forward, 定义在 tinyhttps.cppm

class SseParser {
private:
    std::string buffer_;
    std::string currentEvent_;
    std::string currentData_;
    std::string currentId_;

public:
    // 喂入原始数据，解析出完整事件
    // 返回解析出的事件列表
    std::vector<SseEvent> feed(std::string_view chunk);

    void reset();

private:
    void process_line_(std::string_view line);
    void dispatch_event_(std::vector<SseEvent>& events);
};

} // namespace mcpplibs::tinyhttps
```

---

## 6. 协程支持

### 6.1 Task<T>

最小 coroutine return type，不依赖任何平台 API。

```cpp
// src/coro.cppm
export module mcpplibs.llmapi:coro;

import std;

export namespace mcpplibs::llmapi {

template<typename T>
class Task {
public:
    struct promise_type {
        std::optional<T> value;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task {
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(T val) {
            value = std::move(val);
        }

        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

private:
    std::coroutine_handle<promise_type> handle_;

public:
    explicit Task(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }

    // Move only
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // Awaitable interface
    bool await_ready() const noexcept { return handle_.done(); }
    void await_suspend(std::coroutine_handle<> awaiter) noexcept {
        handle_.resume();
        // 简单实现：同步恢复后返回 awaiter
        // 生产环境可扩展为事件循环调度
        awaiter.resume();
    }
    T await_resume() {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
        return std::move(*handle_.promise().value);
    }

    // 同步等待（阻塞）
    T get() {
        if (!handle_.done()) {
            handle_.resume();
        }
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
        return std::move(*handle_.promise().value);
    }
};

// Task<void> 特化
template<>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception;

        Task get_return_object() {
            return Task {
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

private:
    std::coroutine_handle<promise_type> handle_;

public:
    explicit Task(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }

    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    Task& operator=(Task&&) noexcept;
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    bool await_ready() const noexcept { return handle_.done(); }
    void await_suspend(std::coroutine_handle<> awaiter) noexcept {
        handle_.resume();
        awaiter.resume();
    }
    void await_resume() {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
    }

    void get() {
        if (!handle_.done()) handle_.resume();
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
    }
};

} // namespace mcpplibs::llmapi
```

### 6.2 设计说明

当前 `Task<T>` 是**同步恢复**语义（`await_suspend` 中直接 resume）。这足以支持：
- `co_await` 语法糖
- `.get()` 阻塞获取结果
- 异常传播

未来可扩展为真正的异步调度（基于 `poll()` 的 event loop），但初始版本不引入此复杂度。

---

## 7. 用户 API 示例

### 7.1 基本聊天

```cpp
import mcpplibs.llmapi;

using namespace mcpplibs::llmapi;

int main() {
    // OpenAI
    auto client = Client(openai::OpenAI({
        .apiKey = std::getenv("OPENAI_API_KEY"),
        .model  = "gpt-4o",
    }));

    auto response = client.chat("Hello!");
    std::println("{}", response.text());
}
```

### 7.2 切换 Provider

```cpp
// Anthropic — 只改 Provider，其余代码不变
auto client = Client(anthropic::Anthropic({
    .apiKey = std::getenv("ANTHROPIC_API_KEY"),
    .model  = "claude-sonnet-4-20250514",
}));

auto response = client.chat("Hello!");
std::println("{}", response.text());
```

### 7.3 流式输出

```cpp
auto response = client.chat_stream("讲个笑话", [](std::string_view chunk) {
    std::print("{}", chunk);
});
std::println("\n[tokens: {}]", response.usage.totalTokens);
```

### 7.4 Tool Calling

```cpp
auto params = ChatParams {
    .tools = std::vector<ToolDef>{{
        .name = "get_weather",
        .description = "获取天气信息",
        .inputSchema = R"({"type":"object","properties":{"city":{"type":"string"}},"required":["city"]})",
    }},
    .toolChoice = ToolChoice::Auto,
};

auto response = client.chat("北京今天天气怎么样？", params);

for (auto& call : response.tool_calls()) {
    std::println("调用工具: {} 参数: {}", call.name, call.arguments);

    // 返回工具结果
    client.add_message({
        .role = Role::Tool,
        .content = std::vector<ContentPart>{
            ToolResultContent { .toolUseId = call.id, .content = R"({"temp":"22°C"})" }
        },
    });
}

// 继续对话，模型根据工具结果生成最终回答
auto finalResponse = client.chat("");
```

### 7.5 多模态（图片）

```cpp
client.add_message({
    .role = Role::User,
    .content = std::vector<ContentPart>{
        TextContent { "这张图片里有什么？" },
        ImageContent { .data = "https://example.com/photo.jpg", .isUrl = true },
    },
});

auto response = client.chat("");  // 消息已手动添加，传空触发请求
```

### 7.6 结构化输出

```cpp
auto params = ChatParams {
    .responseFormat = ResponseFormat {
        .type = ResponseFormatType::JsonSchema,
        .schemaName = "person",
        .schema = R"({"type":"object","properties":{"name":{"type":"string"},"age":{"type":"integer"}},"required":["name","age"]})",
    },
};

auto response = client.chat("生成一个虚构人物的信息", params);
auto json = nlohmann::json::parse(response.text());
```

### 7.7 对话序列化

```cpp
// 保存
client.save_conversation("chat_history.json");

// 加载并继续
auto client2 = Client(openai::OpenAI({ ... }));
client2.load_conversation("chat_history.json");
auto response = client2.chat("继续我们之前的对话");
```

### 7.8 协程异步

```cpp
Task<void> async_main() {
    auto client = Client(openai::OpenAI({ ... }));
    auto response = co_await client.chat_async("Hello!");
    std::println("{}", response.text());
}

int main() {
    async_main().get();
}
```

### 7.9 自定义 Headers + 代理

```cpp
auto client = Client(openai::OpenAI({
    .apiKey = "sk-xxx",
    .model  = "gpt-4o",
    .proxy  = "http://proxy.corp.com:8080",
    .customHeaders = {
        {"X-Custom-Header", "value"},
    },
}));
```

---

## 8. 错误处理

### 8.1 策略

使用 C++ 异常 + `std::expected`（可选未来扩展）。

```cpp
export namespace mcpplibs::llmapi {

// HTTP 层错误
struct HttpError : std::runtime_error {
    int statusCode;
    std::string responseBody;

    HttpError(int code, std::string body)
        : std::runtime_error("HTTP " + std::to_string(code))
        , statusCode(code), responseBody(std::move(body)) {}
};

// API 层错误（解析自 Provider 错误响应）
struct ApiError : std::runtime_error {
    std::string type;       // "invalid_request_error", "authentication_error", ...
    std::string param;      // 哪个参数出错（可选）
    int statusCode;

    ApiError(int code, std::string type, std::string message)
        : std::runtime_error(std::move(message))
        , type(std::move(type)), statusCode(code) {}
};

// 连接错误
struct ConnectionError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// TLS 错误
struct TlsError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace mcpplibs::llmapi
```

### 8.2 错误映射

| HTTP 状态码 | 异常类型 | 说明 |
|-------------|----------|------|
| 400 | `ApiError` | 请求参数错误 |
| 401 | `ApiError` | 认证失败 |
| 403 | `ApiError` | 权限不足 |
| 429 | `ApiError` | 限流 |
| 500+ | `ApiError` | 服务端错误 |
| 连接失败 | `ConnectionError` | DNS/TCP 连接失败 |
| TLS 握手失败 | `TlsError` | 证书/握手错误 |
| 响应解析失败 | `std::runtime_error` | JSON 解析等 |

---

## 9. 未来扩展点

| 方向 | 说明 | 优先级 |
|------|------|--------|
| 重试 + 退避 | 指数退避重试策略（可在 Client 层或 HttpClient 层实现） | P1 |
| 真正异步 Event Loop | 基于 poll() 的非阻塞事件循环，替代同步恢复 | P1 |
| 更多 Provider | Gemini, Mistral, 本地 Ollama 等 | P2 |
| 连接池优化 | LRU 淘汰、最大连接数限制 | P2 |
| 日志系统 | 可插拔日志回调 | P2 |
| HTTP/2 | 多路复用（复杂度高，按需） | P3 |

---

## 10. 依赖与编译要求

| 依赖 | 版本 | 用途 | 许可 |
|------|------|------|------|
| mbedtls | 3.6.1 | TLS | Apache 2.0 |
| nlohmann/json | 3.12.0 | JSON 序列化 | MIT |

| 编译器 | 最低版本 | 说明 |
|--------|----------|------|
| GCC | 14+ | C++23 modules + coroutines |
| Clang | 18+ | C++23 modules + coroutines |
| MSVC | 2022 17.5+ | C++23 modules + coroutines |

| 构建系统 | 版本 |
|----------|------|
| xmake | 3.0.0+ |

### 静态链接命令示例

```bash
# musl-gcc 全静态
xmake f -p linux --sdk=/usr/local/musl --links="mbedtls mbedx509 mbedcrypto"
xmake build -m release
```

---

## 附录 A：OpenAI vs Anthropic API 完整对照

| 维度 | OpenAI `/v1/chat/completions` | Anthropic `/v1/messages` |
|------|------|------|
| Auth | `Authorization: Bearer {key}` | `x-api-key: {key}` + `anthropic-version` |
| System | messages 中 `role: "system"` | 顶层 `system` 字段 |
| Roles | system, user, assistant, tool | user, assistant (仅两种) |
| max_tokens | 可选 (`max_completion_tokens`) | **必填** |
| Content | `string` (纯文本) 或 `ContentPart[]` | `string` 或 `ContentBlock[]` |
| Image | `image_url.url` (URL/data URI) | `source: {type, media_type, data}` |
| Tool 定义 | `{type:"function", function:{...}}` | `{name, description, input_schema}` |
| Tool 调用 | `message.tool_calls[]` | content block `type:"tool_use"` |
| Tool 结果 | `role:"tool"` + `tool_call_id` | user msg 中 `type:"tool_result"` block |
| Streaming | `data: {json}` + `data: [DONE]` | `event: {type}\ndata: {json}` |
| 流式文本 | `choices[0].delta.content` | `delta.type:"text_delta"` → `delta.text` |
| finish_reason | stop, length, tool_calls, content_filter | end_turn, max_tokens, tool_use, stop_sequence |
| Usage | prompt_tokens, completion_tokens, total_tokens | input_tokens, output_tokens |
| Embeddings | `/v1/embeddings` | 无 |
| JSON 模式 | `response_format: {type:"json_object"}` | 无原生支持 |
| JSON Schema | `response_format: {type:"json_schema",...}` | 无原生支持 |
