import mcpplibs.llmapi;
import std;

#include <cassert>

using namespace mcpplibs::llmapi;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::println("OPENAI_API_KEY not set, skipping live test");
        return 0;
    }

    auto client = Client(openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    }));

    // Test 1: basic chat
    auto resp = client.chat("Say exactly: HELLO_TEST_OK");
    std::println("Response: {}", resp.text());
    assert(!resp.text().empty());
    assert(resp.usage.totalTokens > 0);
    assert(resp.stopReason == StopReason::EndOfTurn);

    // Test 2: streaming
    client.clear();
    std::string streamed;
    auto resp2 = client.chat_stream("Say exactly: STREAM_OK", [&](std::string_view chunk) {
        streamed += chunk;
        std::print("{}", chunk);
    });
    std::println("");
    assert(!streamed.empty());

    // Test 3: conversation continuity
    auto resp3 = client.chat("What did I just ask you to say?");
    assert(!resp3.text().empty());
    assert(client.conversation().messages.size() == 4); // 2 user + 2 assistant

    std::println("test_openai_live: ALL PASSED");
    return 0;
}
