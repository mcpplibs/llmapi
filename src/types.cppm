export module mcpplibs.llmapi:types;

import std;

namespace mcpplibs::llmapi {

// Roles
export enum class Role { System, User, Assistant, Tool };

// Content blocks (multimodal)
export struct TextContent {
    std::string text;
};

export struct ImageContent {
    std::string data;       // base64 or URL
    std::string mediaType;  // "image/png", "image/jpeg"
    bool isUrl{false};
};

export struct AudioContent {
    std::string data;    // base64
    std::string format;  // "wav", "mp3"
};

export struct ToolUseContent {
    std::string id;
    std::string name;
    std::string inputJson;
};

export struct ToolResultContent {
    std::string toolUseId;
    std::string content;
    bool isError{false};
};

export using ContentPart =
    std::variant<TextContent, ImageContent, AudioContent, ToolUseContent, ToolResultContent>;
export using Content = std::variant<std::string, std::vector<ContentPart>>;

// Message
export struct Message {
    Role role;
    Content content;
    std::string name;

    static Message system(std::string_view text) {
        return Message{.role = Role::System, .content = std::string{text}};
    }

    static Message user(std::string_view text) {
        return Message{.role = Role::User, .content = std::string{text}};
    }

    static Message assistant(std::string_view text) {
        return Message{.role = Role::Assistant, .content = std::string{text}};
    }
};

// Tool definition
export struct ToolDef {
    std::string name;
    std::string description;
    std::string inputSchema;  // JSON Schema string
};

// Tool call (from response)
export struct ToolCall {
    std::string id;
    std::string name;
    std::string arguments;
};

// Tool choice
export enum class ToolChoice { Auto, None, Required };

export struct ToolChoiceForced {
    std::string name;
};

export using ToolChoicePolicy = std::variant<ToolChoice, ToolChoiceForced>;

// Response format
export enum class ResponseFormatType { Text, JsonObject, JsonSchema };

export struct ResponseFormat {
    ResponseFormatType type{ResponseFormatType::Text};
    std::string schemaName;
    std::string schema;
};

// Chat params
export struct ChatParams {
    std::optional<double> temperature;
    std::optional<double> topP;
    std::optional<int> maxTokens;
    std::optional<std::vector<std::string>> stop;
    std::optional<std::vector<ToolDef>> tools;
    std::optional<ToolChoicePolicy> toolChoice;
    std::optional<ResponseFormat> responseFormat;
    std::optional<std::string> extraJson;
};

// Stop reason
export enum class StopReason { EndOfTurn, MaxTokens, ToolUse, ContentFilter, StopSequence };

// Usage
export struct Usage {
    int inputTokens{0};
    int outputTokens{0};
    int totalTokens{0};
};

// Chat response
export struct ChatResponse {
    std::string id;
    std::string model;
    std::vector<ContentPart> content;
    StopReason stopReason;
    Usage usage;

    std::string text() const {
        std::string result;
        for (const auto& part : content) {
            if (auto* t = std::get_if<TextContent>(&part)) {
                result += t->text;
            }
        }
        return result;
    }

    std::vector<ToolCall> tool_calls() const {
        std::vector<ToolCall> calls;
        for (const auto& part : content) {
            if (auto* t = std::get_if<ToolUseContent>(&part)) {
                calls.push_back(ToolCall{
                    .id = t->id,
                    .name = t->name,
                    .arguments = t->inputJson,
                });
            }
        }
        return calls;
    }
};

// Embedding response
export struct EmbeddingResponse {
    std::vector<std::vector<float>> embeddings;
    std::string model;
    Usage usage;
};

// Conversation container
export struct Conversation {
    std::vector<Message> messages;

    void push(Message msg) { messages.push_back(std::move(msg)); }

    void clear() { messages.clear(); }

    int size() const { return static_cast<int>(messages.size()); }
};

}  // namespace mcpplibs::llmapi
