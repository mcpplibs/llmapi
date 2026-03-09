import mcpplibs.llmapi;
import std;

#include <cassert>

using namespace mcpplibs::llmapi;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::println("OPENAI_API_KEY not set, skipping");
        return 0;
    }

    auto provider = openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    });

    auto resp = provider.embed(
        {"Hello world", "How are you"},
        "text-embedding-3-small"
    );

    assert(resp.embeddings.size() == 2);
    assert(!resp.embeddings[0].empty());
    assert(resp.usage.inputTokens > 0);
    std::println("Embedding dim: {}", resp.embeddings[0].size());

    std::println("test_embeddings: ALL PASSED");
    return 0;
}
