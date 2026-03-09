import mcpplibs.llmapi;
import std;

#include <cassert>

using namespace mcpplibs::llmapi;

int main() {
    anthropic::Anthropic provider(anthropic::Config {
        .apiKey = "test-key",
        .model = "claude-sonnet-4-20250514",
    });

    // Test 1: concept satisfaction
    static_assert(Provider<anthropic::Anthropic>);
    static_assert(StreamableProvider<anthropic::Anthropic>);
    // NOT EmbeddableProvider — Anthropic has no embeddings

    // Test 2: name
    assert(provider.name() == "anthropic");

    // Test 3: Client<Anthropic> compiles
    auto client = Client(anthropic::Anthropic({
        .apiKey = "test",
        .model = "claude-sonnet-4-20250514",
    }));
    assert(client.provider().name() == "anthropic");

    std::println("test_anthropic_serialize: ALL PASSED");
    return 0;
}
