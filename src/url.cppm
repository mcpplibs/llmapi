export module mcpplibs.llmapi.url;

import std;

export namespace mcpplibs::llmapi {

struct URL {
    // Models
    inline static constexpr std::string_view OpenAI { "https://api.openai.com/v1" };
    inline static constexpr std::string_view Anthropic { "https://api.anthropic.com/v1" };
    inline static constexpr std::string_view DeepSeek { "https://api.deepseek.com/v1" };

    // Providers
    inline static constexpr std::string_view OpenRouter { "https://openrouter.ai/api/v1" };
    inline static constexpr std::string_view Poe { "https://api.poe.com/v1" };
};

} // namespace mcpplibs::llmapi