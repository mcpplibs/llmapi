import mcpplibs.llmapi;
import std;

#include <cassert>
#include "../test_print.hpp"

using namespace mcpplibs::llmapi;

int main() {
    auto apiKey = std::getenv("ANTHROPIC_API_KEY");
    if (!apiKey) {
        println("ANTHROPIC_API_KEY not set, skipping live test");
        return 0;
    }

    auto client = Client(anthropic::Anthropic({
        .apiKey = apiKey,
        .model = "claude-haiku-4-5-20251001",
    }));

    // Test 1: basic chat
    auto resp = client.chat("Say exactly: HELLO_TEST_OK");
    println("Response: ", resp.text());
    assert(!resp.text().empty());
    assert(resp.usage.inputTokens > 0);

    // Test 2: system message
    client.clear();
    client.system("Always respond with exactly one word.");
    auto resp2 = client.chat("What color is the sky?");
    println("System test: ", resp2.text());

    // Test 3: streaming
    client.clear();
    std::string streamed;
    auto resp3 = client.chat_stream("Say exactly: STREAM_OK", [&](std::string_view chunk) {
        streamed += chunk;
        print(chunk);
    });
    println();
    assert(!streamed.empty());

    println("test_anthropic_live: ALL PASSED");
    return 0;
}
