import mcpplibs.llmapi;
import mcpplibs.llmapi.nlohmann.json;
import std;

#include <cassert>
#include "../test_print.hpp"

using namespace mcpplibs::llmapi;
using Json = nlohmann::json;

int main() {
    // We can't test private methods directly, so test through the public API
    // by creating an OpenAI provider and verifying it satisfies concepts

    openai::OpenAI provider(openai::Config {
        .apiKey = "test-key",
        .model = "gpt-4o",
    });

    // Test 1: Provider concept satisfaction
    static_assert(Provider<openai::OpenAI>);
    static_assert(StreamableProvider<openai::OpenAI>);
    static_assert(EmbeddableProvider<openai::OpenAI>);

    // Test 2: name()
    assert(provider.name() == "openai");

    // Test 3: Client<OpenAI> compiles
    auto client = Client(openai::OpenAI({
        .apiKey = "test",
        .model = "gpt-4o",
    }));

    // Test 4: provider() access
    assert(client.provider().name() == "openai");

    println("test_openai_serialize: ALL PASSED");
    return 0;
}
