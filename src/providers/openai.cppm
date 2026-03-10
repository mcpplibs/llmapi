module;

#include <cassert>

export module mcpplibs.llmapi:openai;

export import :url;

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
    std::string organization;
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
            .keepAlive = true,
          })
    {
    }

    // Non-copyable (HttpClient owns TLS connections)
    OpenAI(const OpenAI&) = delete;
    OpenAI& operator=(const OpenAI&) = delete;
    OpenAI(OpenAI&&) = default;
    OpenAI& operator=(OpenAI&&) = default;

    // Provider concept
    std::string_view name() const { return "openai"; }

    ChatResponse chat(const std::vector<Message>& messages, const ChatParams& params) {
        auto payload = build_payload_(messages, params, false);
        auto request = build_request_("/chat/completions", payload);
        auto response = http_.send(request);
        if (!response.ok()) {
            throw std::runtime_error("OpenAI API error: " +
                std::to_string(response.statusCode) + " " + response.body);
        }
        return parse_response_(Json::parse(response.body));
    }

    Task<ChatResponse> chat_async(const std::vector<Message>& messages, const ChatParams& params) {
        co_return chat(messages, params);
    }

    // StreamableProvider
    ChatResponse chat_stream(const std::vector<Message>& messages, const ChatParams& params,
                             std::function<void(std::string_view)> callback) {
        auto payload = build_payload_(messages, params, true);
        auto request = build_request_("/chat/completions", payload);

        ChatResponse result;
        std::string fullContent;
        std::string currentToolId;
        std::string currentToolName;
        std::string currentToolArgs;
        bool inToolCall = false;

        auto sseResponse = http_.send_stream(request, [&](const tinyhttps::SseEvent& event) -> bool {
            if (event.data == "[DONE]") {
                return false;
            }
            try {
                auto chunk = Json::parse(event.data);
                if (result.id.empty() && chunk.contains("id")) {
                    result.id = chunk["id"].get<std::string>();
                }
                if (result.model.empty() && chunk.contains("model")) {
                    result.model = chunk["model"].get<std::string>();
                }
                if (chunk.contains("choices") && !chunk["choices"].empty()) {
                    const auto& choice = chunk["choices"][0];
                    if (choice.contains("delta")) {
                        const auto& delta = choice["delta"];
                        if (delta.contains("content") && !delta["content"].is_null()) {
                            std::string content = delta["content"].get<std::string>();
                            fullContent += content;
                            callback(content);
                        }
                        if (delta.contains("tool_calls")) {
                            for (const auto& tc : delta["tool_calls"]) {
                                if (tc.contains("id")) {
                                    // New tool call starting — flush previous if any
                                    if (inToolCall) {
                                        result.content.push_back(ToolUseContent {
                                            .id = currentToolId,
                                            .name = currentToolName,
                                            .inputJson = currentToolArgs,
                                        });
                                    }
                                    currentToolId = tc["id"].get<std::string>();
                                    currentToolName = tc.contains("function") && tc["function"].contains("name")
                                        ? tc["function"]["name"].get<std::string>() : "";
                                    currentToolArgs = tc.contains("function") && tc["function"].contains("arguments")
                                        ? tc["function"]["arguments"].get<std::string>() : "";
                                    inToolCall = true;
                                } else {
                                    // Continuation of existing tool call
                                    if (tc.contains("function") && tc["function"].contains("arguments")) {
                                        currentToolArgs += tc["function"]["arguments"].get<std::string>();
                                    }
                                }
                            }
                        }
                    }
                    if (choice.contains("finish_reason") && !choice["finish_reason"].is_null()) {
                        result.stopReason = parse_stop_reason_(choice["finish_reason"].get<std::string>());
                    }
                }
                if (chunk.contains("usage") && !chunk["usage"].is_null()) {
                    const auto& usage = chunk["usage"];
                    result.usage.inputTokens = usage.value("prompt_tokens", 0);
                    result.usage.outputTokens = usage.value("completion_tokens", 0);
                    result.usage.totalTokens = result.usage.inputTokens + result.usage.outputTokens;
                }
            } catch (const Json::exception&) {
                // Skip malformed chunks
            }
            return true;
        });

        // Flush last tool call if any
        if (inToolCall) {
            result.content.push_back(ToolUseContent {
                .id = currentToolId,
                .name = currentToolName,
                .inputJson = currentToolArgs,
            });
        }

        // Add text content if present
        if (!fullContent.empty()) {
            result.content.insert(result.content.begin(), TextContent { .text = fullContent });
        }

        if (!sseResponse.ok()) {
            throw std::runtime_error("OpenAI API stream error: " +
                std::to_string(sseResponse.statusCode) + " " + sseResponse.statusText);
        }

        return result;
    }

    Task<ChatResponse> chat_stream_async(const std::vector<Message>& messages, const ChatParams& params,
                                          std::function<void(std::string_view)> callback) {
        co_return chat_stream(messages, params, std::move(callback));
    }

    // EmbeddableProvider
    EmbeddingResponse embed(const std::vector<std::string>& inputs, std::string_view model) {
        Json payload;
        payload["model"] = std::string(model);
        payload["input"] = inputs;

        auto request = build_request_("/embeddings", payload);
        auto response = http_.send(request);
        if (!response.ok()) {
            throw std::runtime_error("OpenAI embeddings error: " +
                std::to_string(response.statusCode) + " " + response.body);
        }

        auto json = Json::parse(response.body);
        EmbeddingResponse result;
        result.model = json.value("model", std::string(model));

        for (const auto& item : json["data"]) {
            std::vector<float> vec;
            for (const auto& val : item["embedding"]) {
                vec.push_back(val.get<float>());
            }
            result.embeddings.push_back(std::move(vec));
        }

        if (json.contains("usage")) {
            result.usage.inputTokens = json["usage"].value("prompt_tokens", 0);
            result.usage.totalTokens = json["usage"].value("total_tokens", 0);
        }

        return result;
    }

private:
    // Serialization
    Json serialize_messages_(const std::vector<Message>& messages) const {
        Json arr = Json::array();
        for (const auto& msg : messages) {
            arr.push_back(serialize_message_(msg));
        }
        return arr;
    }

    Json serialize_message_(const Message& msg) const {
        Json j;
        j["role"] = role_string_(msg.role);

        std::visit([&](const auto& c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j["content"] = c;
            } else {
                // vector<ContentPart>
                Json parts = Json::array();
                for (const auto& part : c) {
                    std::visit([&](const auto& p) {
                        using P = std::decay_t<decltype(p)>;
                        if constexpr (std::is_same_v<P, TextContent>) {
                            parts.push_back(Json{{"type", "text"}, {"text", p.text}});
                        } else if constexpr (std::is_same_v<P, ImageContent>) {
                            Json imgUrl;
                            if (p.isUrl) {
                                imgUrl["url"] = p.data;
                            } else {
                                imgUrl["url"] = "data:" + p.mediaType + ";base64," + p.data;
                            }
                            parts.push_back(Json{{"type", "image_url"}, {"image_url", imgUrl}});
                        } else if constexpr (std::is_same_v<P, ToolUseContent>) {
                            // Tool use in assistant messages — handled via tool_calls field
                        } else if constexpr (std::is_same_v<P, ToolResultContent>) {
                            // Tool results go as separate role=tool messages
                        }
                    }, part);
                }
                if (!parts.empty()) {
                    j["content"] = parts;
                }
            }
        }, msg.content);

        // Handle tool role: add tool_call_id
        if (msg.role == Role::Tool) {
            // Try to extract tool_call_id from ToolResultContent
            if (auto* parts = std::get_if<std::vector<ContentPart>>(&msg.content)) {
                for (const auto& part : *parts) {
                    if (auto* tr = std::get_if<ToolResultContent>(&part)) {
                        j["tool_call_id"] = tr->toolUseId;
                        j["content"] = tr->content;
                        break;
                    }
                }
            }
        }

        // Handle assistant messages with tool_calls
        if (msg.role == Role::Assistant) {
            if (auto* parts = std::get_if<std::vector<ContentPart>>(&msg.content)) {
                Json toolCalls = Json::array();
                std::string textContent;
                for (const auto& part : *parts) {
                    if (auto* tu = std::get_if<ToolUseContent>(&part)) {
                        Json tc;
                        tc["id"] = tu->id;
                        tc["type"] = "function";
                        tc["function"] = Json{
                            {"name", tu->name},
                            {"arguments", tu->inputJson},
                        };
                        toolCalls.push_back(tc);
                    } else if (auto* t = std::get_if<TextContent>(&part)) {
                        textContent += t->text;
                    }
                }
                if (!toolCalls.empty()) {
                    j["tool_calls"] = toolCalls;
                }
                if (!textContent.empty()) {
                    j["content"] = textContent;
                } else if (toolCalls.empty()) {
                    // Keep array content if no special handling needed
                } else {
                    j["content"] = nullptr;
                }
            }
        }

        return j;
    }

    Json build_payload_(const std::vector<Message>& messages, const ChatParams& params, bool stream) const {
        Json payload;
        payload["model"] = config_.model;
        payload["messages"] = serialize_messages_(messages);

        if (stream) {
            payload["stream"] = true;
        }

        if (params.temperature.has_value()) {
            payload["temperature"] = *params.temperature;
        }
        if (params.topP.has_value()) {
            payload["top_p"] = *params.topP;
        }
        if (params.maxTokens.has_value()) {
            payload["max_completion_tokens"] = *params.maxTokens;
        }
        if (params.stop.has_value()) {
            payload["stop"] = *params.stop;
        }

        // Tools
        if (params.tools.has_value() && !params.tools->empty()) {
            Json tools = Json::array();
            for (const auto& tool : *params.tools) {
                Json t;
                t["type"] = "function";
                t["function"] = Json{
                    {"name", tool.name},
                    {"description", tool.description},
                };
                if (!tool.inputSchema.empty()) {
                    t["function"]["parameters"] = Json::parse(tool.inputSchema);
                }
                tools.push_back(t);
            }
            payload["tools"] = tools;
        }

        // Tool choice
        if (params.toolChoice.has_value()) {
            std::visit([&](const auto& tc) {
                using T = std::decay_t<decltype(tc)>;
                if constexpr (std::is_same_v<T, ToolChoice>) {
                    switch (tc) {
                        case ToolChoice::Auto: payload["tool_choice"] = "auto"; break;
                        case ToolChoice::None: payload["tool_choice"] = "none"; break;
                        case ToolChoice::Required: payload["tool_choice"] = "required"; break;
                    }
                } else if constexpr (std::is_same_v<T, ToolChoiceForced>) {
                    payload["tool_choice"] = Json{
                        {"type", "function"},
                        {"function", Json{{"name", tc.name}}},
                    };
                }
            }, *params.toolChoice);
        }

        // Response format
        if (params.responseFormat.has_value()) {
            const auto& rf = *params.responseFormat;
            switch (rf.type) {
                case ResponseFormatType::Text:
                    payload["response_format"] = Json{{"type", "text"}};
                    break;
                case ResponseFormatType::JsonObject:
                    payload["response_format"] = Json{{"type", "json_object"}};
                    break;
                case ResponseFormatType::JsonSchema: {
                    Json schemaObj;
                    schemaObj["type"] = "json_schema";
                    Json jsonSchema;
                    jsonSchema["name"] = rf.schemaName;
                    if (!rf.schema.empty()) {
                        jsonSchema["schema"] = Json::parse(rf.schema);
                    }
                    schemaObj["json_schema"] = jsonSchema;
                    payload["response_format"] = schemaObj;
                    break;
                }
            }
        }

        // Extra JSON merge
        if (params.extraJson.has_value() && !params.extraJson->empty()) {
            auto extra = Json::parse(*params.extraJson);
            payload.merge_patch(extra);
        }

        return payload;
    }

    // Deserialization
    ChatResponse parse_response_(const Json& json) const {
        ChatResponse result;

        result.id = json.value("id", "");
        result.model = json.value("model", "");

        if (json.contains("choices") && !json["choices"].empty()) {
            const auto& choice = json["choices"][0];
            if (choice.contains("message")) {
                const auto& msg = choice["message"];
                if (msg.contains("content") && !msg["content"].is_null()) {
                    result.content.push_back(TextContent {
                        .text = msg["content"].get<std::string>(),
                    });
                }
                if (msg.contains("tool_calls")) {
                    for (const auto& tc : msg["tool_calls"]) {
                        result.content.push_back(ToolUseContent {
                            .id = tc.value("id", ""),
                            .name = tc["function"].value("name", ""),
                            .inputJson = tc["function"].value("arguments", ""),
                        });
                    }
                }
            }
            if (choice.contains("finish_reason") && !choice["finish_reason"].is_null()) {
                result.stopReason = parse_stop_reason_(choice["finish_reason"].get<std::string>());
            }
        }

        if (json.contains("usage")) {
            const auto& usage = json["usage"];
            result.usage.inputTokens = usage.value("prompt_tokens", 0);
            result.usage.outputTokens = usage.value("completion_tokens", 0);
            result.usage.totalTokens = result.usage.inputTokens + result.usage.outputTokens;
        }

        return result;
    }

    static StopReason parse_stop_reason_(const std::string& reason) {
        if (reason == "stop") return StopReason::EndOfTurn;
        if (reason == "length") return StopReason::MaxTokens;
        if (reason == "tool_calls") return StopReason::ToolUse;
        if (reason == "content_filter") return StopReason::ContentFilter;
        return StopReason::EndOfTurn;
    }

    static std::string role_string_(Role role) {
        switch (role) {
            case Role::System: return "system";
            case Role::User: return "user";
            case Role::Assistant: return "assistant";
            case Role::Tool: return "tool";
        }
        return "user";
    }

    // HTTP helpers
    tinyhttps::HttpRequest build_request_(std::string_view endpoint, const Json& payload) const {
        tinyhttps::HttpRequest req;
        req.method = tinyhttps::Method::POST;
        req.url = config_.baseUrl + std::string(endpoint);
        req.body = payload.dump();

        req.headers["Content-Type"] = "application/json";
        req.headers["Authorization"] = "Bearer " + config_.apiKey;

        if (!config_.organization.empty()) {
            req.headers["OpenAI-Organization"] = config_.organization;
        }

        for (const auto& [key, value] : config_.customHeaders) {
            req.headers[key] = value;
        }

        return req;
    }
};

} // namespace mcpplibs::llmapi::openai
