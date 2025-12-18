export module mcpplibs.llmapi;

import std;

import nlohmann.json;

import mcpplibs.llmapi.url;
import mcpplibs.llmapi.openai;

namespace mcpplibs::llmapi {
    export using OpenAI = openai::OpenAI;
    export using Client = openai::OpenAI;
    export using URL = llmapi::URL;
    export using Json = nlohmann::json;
} // namespace mcpplibs::llmapi