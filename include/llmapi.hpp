#pragma once

#include <json.hpp>
#include "llmapi/url.hpp"
#include "llmapi/openai.hpp"

namespace mcpplibs::llmapi {
    using OpenAI = openai::OpenAI;
    using Client = openai::OpenAI;
    using URL = llmapi::URL;
    using Json = nlohmann::json;
} // namespace mcpplibs::llmapi