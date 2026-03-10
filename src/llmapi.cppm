export module mcpplibs.llmapi;

export import :types;
export import :url;
export import :coro;
export import :provider;
export import :client;
export import :openai;
export import :anthropic;
export import :errors;

import std;

import mcpplibs.llmapi.nlohmann.json;

namespace mcpplibs::llmapi {
    export inline constexpr std::string_view VERSION { "0.1.0" };
    export using OpenAI = openai::OpenAI;
    export using Config = openai::Config;
    export using Anthropic = anthropic::Anthropic;
    export using AnthropicConfig = anthropic::Config;
    export using URL = llmapi::URL;
    export using Json = nlohmann::json;
} // namespace mcpplibs::llmapi
