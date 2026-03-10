module;

#include <cassert>

export module mcpplibs.llmapi:anthropic;

export import :url;

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
    int defaultMaxTokens { 4096 };          // REQUIRED by Anthropic
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
            .keepAlive = true,
          })
    {
    }

    // Non-copyable (HttpClient owns TLS connections)
    Anthropic(const Anthropic&) = delete;
    Anthropic& operator=(const Anthropic&) = delete;
    Anthropic(Anthropic&&) = default;
    Anthropic& operator=(Anthropic&&) = default;

    // Provider concept
    std::string_view name() const { return "anthropic"; }

    ChatResponse chat(const std::vector<Message>& messages, const ChatParams& params) {
        auto payload = build_payload_(messages, params, false);
        auto request = build_request_("/messages", payload);
        auto response = http_.send(request);
        if (!response.ok()) {
            throw std::runtime_error("Anthropic API error: " +
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
        auto request = build_request_("/messages", payload);

        ChatResponse result;
        std::string fullContent;
        std::string currentToolId;
        std::string currentToolName;
        std::string currentToolArgs;
        bool inToolCall = false;

        auto sseResponse = http_.send_stream(request, [&](const tinyhttps::SseEvent& event) -> bool {
            // Anthropic uses named events
            if (event.event == "message_stop") {
                return false;
            }
            if (event.event == "ping") {
                return true;
            }

            try {
                auto chunk = Json::parse(event.data);

                if (event.event == "message_start") {
                    if (chunk.contains("message")) {
                        const auto& msg = chunk["message"];
                        if (msg.contains("id")) {
                            result.id = msg["id"].get<std::string>();
                        }
                        if (msg.contains("model")) {
                            result.model = msg["model"].get<std::string>();
                        }
                        if (msg.contains("usage")) {
                            result.usage.inputTokens = msg["usage"].value("input_tokens", 0);
                        }
                    }
                } else if (event.event == "content_block_start") {
                    if (chunk.contains("content_block")) {
                        const auto& block = chunk["content_block"];
                        auto type = block.value("type", "");
                        if (type == "tool_use") {
                            // Flush previous tool call if any
                            if (inToolCall) {
                                result.content.push_back(ToolUseContent {
                                    .id = currentToolId,
                                    .name = currentToolName,
                                    .inputJson = currentToolArgs,
                                });
                            }
                            currentToolId = block.value("id", "");
                            currentToolName = block.value("name", "");
                            currentToolArgs = "";
                            inToolCall = true;
                        }
                    }
                } else if (event.event == "content_block_delta") {
                    if (chunk.contains("delta")) {
                        const auto& delta = chunk["delta"];
                        auto type = delta.value("type", "");
                        if (type == "text_delta") {
                            std::string text = delta.value("text", "");
                            fullContent += text;
                            callback(text);
                        } else if (type == "input_json_delta") {
                            currentToolArgs += delta.value("partial_json", "");
                        }
                    }
                } else if (event.event == "content_block_stop") {
                    // Block complete — nothing special needed here
                } else if (event.event == "message_delta") {
                    if (chunk.contains("delta")) {
                        const auto& delta = chunk["delta"];
                        if (delta.contains("stop_reason") && !delta["stop_reason"].is_null()) {
                            result.stopReason = parse_stop_reason_(delta["stop_reason"].get<std::string>());
                        }
                    }
                    if (chunk.contains("usage")) {
                        result.usage.outputTokens = chunk["usage"].value("output_tokens", 0);
                        result.usage.totalTokens = result.usage.inputTokens + result.usage.outputTokens;
                    }
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
            throw std::runtime_error("Anthropic API stream error: " +
                std::to_string(sseResponse.statusCode) + " " + sseResponse.statusText);
        }

        return result;
    }

    Task<ChatResponse> chat_stream_async(const std::vector<Message>& messages, const ChatParams& params,
                                          std::function<void(std::string_view)> callback) {
        co_return chat_stream(messages, params, std::move(callback));
    }

    // NOTE: No embed() — Anthropic doesn't have an embeddings API

private:
    // Serialization — extract system message and serialize remaining messages
    std::pair<std::string, Json> extract_system_and_messages_(const std::vector<Message>& messages) const {
        std::string systemText;
        Json arr = Json::array();

        for (const auto& msg : messages) {
            if (msg.role == Role::System) {
                // Extract system content as top-level field
                if (auto* text = std::get_if<std::string>(&msg.content)) {
                    if (!systemText.empty()) systemText += "\n";
                    systemText += *text;
                } else if (auto* parts = std::get_if<std::vector<ContentPart>>(&msg.content)) {
                    for (const auto& part : *parts) {
                        if (auto* t = std::get_if<TextContent>(&part)) {
                            if (!systemText.empty()) systemText += "\n";
                            systemText += t->text;
                        }
                    }
                }
                continue;
            }

            if (msg.role == Role::Tool) {
                // Convert Tool messages to user messages with tool_result content blocks
                Json j;
                j["role"] = "user";
                Json contentArr = Json::array();
                if (auto* parts = std::get_if<std::vector<ContentPart>>(&msg.content)) {
                    for (const auto& part : *parts) {
                        if (auto* tr = std::get_if<ToolResultContent>(&part)) {
                            Json block;
                            block["type"] = "tool_result";
                            block["tool_use_id"] = tr->toolUseId;
                            block["content"] = tr->content;
                            if (tr->isError) {
                                block["is_error"] = true;
                            }
                            contentArr.push_back(block);
                        }
                    }
                }
                j["content"] = contentArr;
                arr.push_back(j);
                continue;
            }

            arr.push_back(serialize_message_(msg));
        }

        return {systemText, arr};
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
                bool hasOnlyText = true;
                std::string textOnly;
                for (const auto& part : c) {
                    if (!std::holds_alternative<TextContent>(part)) {
                        hasOnlyText = false;
                        break;
                    }
                }

                if (hasOnlyText && c.size() == 1) {
                    // Single text block — use simple string
                    j["content"] = std::get<TextContent>(c[0]).text;
                } else {
                    // Multimodal or multi-block — use content array
                    Json parts = Json::array();
                    for (const auto& part : c) {
                        std::visit([&](const auto& p) {
                            using P = std::decay_t<decltype(p)>;
                            if constexpr (std::is_same_v<P, TextContent>) {
                                parts.push_back(Json{{"type", "text"}, {"text", p.text}});
                            } else if constexpr (std::is_same_v<P, ImageContent>) {
                                if (p.isUrl) {
                                    parts.push_back(Json{
                                        {"type", "image"},
                                        {"source", Json{{"type", "url"}, {"url", p.data}}},
                                    });
                                } else {
                                    parts.push_back(Json{
                                        {"type", "image"},
                                        {"source", Json{
                                            {"type", "base64"},
                                            {"media_type", p.mediaType},
                                            {"data", p.data},
                                        }},
                                    });
                                }
                            } else if constexpr (std::is_same_v<P, ToolUseContent>) {
                                // Tool use in assistant messages — inline content blocks
                                Json block;
                                block["type"] = "tool_use";
                                block["id"] = p.id;
                                block["name"] = p.name;
                                if (!p.inputJson.empty()) {
                                    block["input"] = Json::parse(p.inputJson);
                                } else {
                                    block["input"] = Json::object();
                                }
                                parts.push_back(block);
                            } else if constexpr (std::is_same_v<P, ToolResultContent>) {
                                // Tool results in user messages
                                Json block;
                                block["type"] = "tool_result";
                                block["tool_use_id"] = p.toolUseId;
                                block["content"] = p.content;
                                if (p.isError) {
                                    block["is_error"] = true;
                                }
                                parts.push_back(block);
                            }
                        }, part);
                    }
                    if (!parts.empty()) {
                        j["content"] = parts;
                    }
                }
            }
        }, msg.content);

        return j;
    }

    Json build_payload_(const std::vector<Message>& messages, const ChatParams& params, bool stream) const {
        Json payload;
        payload["model"] = config_.model;

        auto [systemText, msgArray] = extract_system_and_messages_(messages);

        if (!systemText.empty()) {
            payload["system"] = systemText;
        }
        payload["messages"] = msgArray;

        // max_tokens is REQUIRED by Anthropic
        payload["max_tokens"] = params.maxTokens.value_or(config_.defaultMaxTokens);

        if (stream) {
            payload["stream"] = true;
        }

        if (params.temperature.has_value()) {
            payload["temperature"] = *params.temperature;
        }
        if (params.topP.has_value()) {
            payload["top_p"] = *params.topP;
        }
        if (params.stop.has_value()) {
            payload["stop_sequences"] = *params.stop;
        }

        // Tools — Anthropic format (no function wrapper)
        if (params.tools.has_value() && !params.tools->empty()) {
            Json tools = Json::array();
            for (const auto& tool : *params.tools) {
                Json t;
                t["name"] = tool.name;
                t["description"] = tool.description;
                if (!tool.inputSchema.empty()) {
                    t["input_schema"] = Json::parse(tool.inputSchema);
                } else {
                    t["input_schema"] = Json{{"type", "object"}};
                }
                tools.push_back(t);
            }
            payload["tools"] = tools;
        }

        // Tool choice — Anthropic format
        if (params.toolChoice.has_value()) {
            std::visit([&](const auto& tc) {
                using T = std::decay_t<decltype(tc)>;
                if constexpr (std::is_same_v<T, ToolChoice>) {
                    switch (tc) {
                        case ToolChoice::Auto: payload["tool_choice"] = Json{{"type", "auto"}}; break;
                        case ToolChoice::None: payload["tool_choice"] = Json{{"type", "none"}}; break;
                        case ToolChoice::Required: payload["tool_choice"] = Json{{"type", "any"}}; break;
                    }
                } else if constexpr (std::is_same_v<T, ToolChoiceForced>) {
                    payload["tool_choice"] = Json{
                        {"type", "tool"},
                        {"name", tc.name},
                    };
                }
            }, *params.toolChoice);
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

        // Anthropic returns content as array of blocks
        if (json.contains("content") && json["content"].is_array()) {
            for (const auto& block : json["content"]) {
                auto type = block.value("type", "");
                if (type == "text") {
                    result.content.push_back(TextContent {
                        .text = block.value("text", ""),
                    });
                } else if (type == "tool_use") {
                    std::string inputJson;
                    if (block.contains("input")) {
                        inputJson = block["input"].dump();
                    }
                    result.content.push_back(ToolUseContent {
                        .id = block.value("id", ""),
                        .name = block.value("name", ""),
                        .inputJson = inputJson,
                    });
                }
            }
        }

        if (json.contains("stop_reason") && !json["stop_reason"].is_null()) {
            result.stopReason = parse_stop_reason_(json["stop_reason"].get<std::string>());
        }

        if (json.contains("usage")) {
            const auto& usage = json["usage"];
            result.usage.inputTokens = usage.value("input_tokens", 0);
            result.usage.outputTokens = usage.value("output_tokens", 0);
            result.usage.totalTokens = result.usage.inputTokens + result.usage.outputTokens;
        }

        return result;
    }

    static StopReason parse_stop_reason_(const std::string& reason) {
        if (reason == "end_turn") return StopReason::EndOfTurn;
        if (reason == "max_tokens") return StopReason::MaxTokens;
        if (reason == "tool_use") return StopReason::ToolUse;
        if (reason == "stop_sequence") return StopReason::StopSequence;
        return StopReason::EndOfTurn;
    }

    static std::string role_string_(Role role) {
        switch (role) {
            case Role::System: return "system";
            case Role::User: return "user";
            case Role::Assistant: return "assistant";
            case Role::Tool: return "user";  // Anthropic: tool results go in user messages
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
        req.headers["x-api-key"] = config_.apiKey;
        req.headers["anthropic-version"] = config_.version;

        for (const auto& [key, value] : config_.customHeaders) {
            req.headers[key] = value;
        }

        return req;
    }
};

} // namespace mcpplibs::llmapi::anthropic
