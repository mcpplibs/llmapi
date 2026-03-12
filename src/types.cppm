export module mcpplibs.llmapi:types;

import std;
import mcpplibs.llmapi.nlohmann.json;

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

export struct CacheControl {
    std::string type {"ephemeral"};
};

// Message
export struct Message {
    Role role;
    Content content;
    std::string name;
    std::optional<CacheControl> cacheControl;

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
    int cacheCreationTokens{0};
    int cacheReadTokens{0};
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

    void save(std::string_view filePath) const;
    static Conversation load(std::string_view filePath);
};

// -- Serialization helpers (internal) --

using Json = nlohmann::json;

inline std::string roleToString(Role r) {
    switch (r) {
        case Role::System:    return "system";
        case Role::User:      return "user";
        case Role::Assistant: return "assistant";
        case Role::Tool:      return "tool";
    }
    return "user";
}

inline Role roleFromString(const std::string& s) {
    if (s == "system")    return Role::System;
    if (s == "assistant") return Role::Assistant;
    if (s == "tool")      return Role::Tool;
    return Role::User;
}

inline Json contentPartToJson(const ContentPart& part) {
    return std::visit([](const auto& p) -> Json {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, TextContent>) {
            return Json{{"type", "text"}, {"text", p.text}};
        } else if constexpr (std::is_same_v<T, ImageContent>) {
            return Json{{"type", "image"}, {"data", p.data}, {"mediaType", p.mediaType}, {"isUrl", p.isUrl}};
        } else if constexpr (std::is_same_v<T, AudioContent>) {
            return Json{{"type", "audio"}, {"data", p.data}, {"format", p.format}};
        } else if constexpr (std::is_same_v<T, ToolUseContent>) {
            return Json{{"type", "tool_use"}, {"id", p.id}, {"name", p.name}, {"inputJson", p.inputJson}};
        } else if constexpr (std::is_same_v<T, ToolResultContent>) {
            return Json{{"type", "tool_result"}, {"toolUseId", p.toolUseId}, {"content", p.content}, {"isError", p.isError}};
        }
    }, part);
}

inline ContentPart contentPartFromJson(const Json& j) {
    auto type = j.at("type").get<std::string>();
    if (type == "text") {
        return TextContent{.text = j.at("text").get<std::string>()};
    } else if (type == "image") {
        return ImageContent{
            .data = j.at("data").get<std::string>(),
            .mediaType = j.at("mediaType").get<std::string>(),
            .isUrl = j.value("isUrl", false),
        };
    } else if (type == "audio") {
        return AudioContent{
            .data = j.at("data").get<std::string>(),
            .format = j.at("format").get<std::string>(),
        };
    } else if (type == "tool_use") {
        return ToolUseContent{
            .id = j.at("id").get<std::string>(),
            .name = j.at("name").get<std::string>(),
            .inputJson = j.at("inputJson").get<std::string>(),
        };
    } else if (type == "tool_result") {
        return ToolResultContent{
            .toolUseId = j.at("toolUseId").get<std::string>(),
            .content = j.at("content").get<std::string>(),
            .isError = j.value("isError", false),
        };
    }
    return TextContent{.text = ""};
}

inline Json messageToJson(const Message& msg) {
    Json j;
    j["role"] = roleToString(msg.role);
    std::visit([&j](const auto& c) {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j["content"] = c;
        } else {
            Json arr = Json::array();
            for (const auto& part : c) {
                arr.push_back(contentPartToJson(part));
            }
            j["content"] = arr;
        }
    }, msg.content);
    if (msg.cacheControl) {
        j["cache_control"] = Json{{"type", msg.cacheControl->type}};
    }
    return j;
}

inline Message messageFromJson(const Json& j) {
    Message msg;
    msg.role = roleFromString(j.at("role").get<std::string>());
    const auto& c = j.at("content");
    if (c.is_string()) {
        msg.content = c.get<std::string>();
    } else if (c.is_array()) {
        std::vector<ContentPart> parts;
        for (const auto& elem : c) {
            parts.push_back(contentPartFromJson(elem));
        }
        msg.content = std::move(parts);
    }
    if (j.contains("cache_control") && j["cache_control"].is_object()) {
        msg.cacheControl = CacheControl{.type = j["cache_control"].value("type", "ephemeral")};
    }
    return msg;
}

void Conversation::save(std::string_view filePath) const {
    Json j;
    j["messages"] = Json::array();
    for (const auto& msg : messages) {
        j["messages"].push_back(messageToJson(msg));
    }
    std::ofstream out{std::string{filePath}};
    out << j.dump(2, ' ', false, Json::error_handler_t::replace);
}

Conversation Conversation::load(std::string_view filePath) {
    std::ifstream in{std::string{filePath}};
    std::string content{std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
    auto j = Json::parse(content);
    Conversation conv;
    for (const auto& msgJson : j.at("messages")) {
        conv.push(messageFromJson(msgJson));
    }
    return conv;
}

}  // namespace mcpplibs::llmapi
