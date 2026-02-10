export module mcpplibs.llmapi;

export import :url;
export import :json;
export import :openai;

import std;

namespace mcpplibs::llmapi {
    export using OpenAI = openai::OpenAI;
    export using Client = openai::OpenAI;
    export using URL = llmapi::URL;
    export using Json = nlohmann::json;
} // namespace mcpplibs::llmapi