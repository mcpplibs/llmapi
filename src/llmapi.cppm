export module mcpplibs.llmapi;

export import :types;
export import :url;
export import :coro;
export import :provider;
export import :openai;

import std;

import mcpplibs.llmapi.nlohmann.json;

namespace mcpplibs::llmapi {
    export using OpenAI = openai::OpenAI;
    export using Client = openai::OpenAI;
    export using URL = llmapi::URL;
    export using Json = nlohmann::json;
} // namespace mcpplibs::llmapi