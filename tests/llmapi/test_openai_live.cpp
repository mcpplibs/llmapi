import mcpplibs.llmapi;
import std;

#include <cassert>
#include "../test_print.hpp"

using namespace mcpplibs::llmapi;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        println("OPENAI_API_KEY not set, skipping live test");
        return 0;
    }

    auto client = Client(openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    }));

    // Test 1: basic chat
    auto resp = client.chat("Say exactly: HELLO_TEST_OK");
    println("Response: ", resp.text());
    assert(!resp.text().empty());
    assert(resp.usage.totalTokens > 0);
    assert(resp.stopReason == StopReason::EndOfTurn);

    // Test 2: streaming
    client.clear();
    std::string streamed;
    auto resp2 = client.chat_stream("Say exactly: STREAM_OK", [&](std::string_view chunk) {
        streamed += chunk;
        print(chunk);
    });
    println();
    assert(!streamed.empty());

    // Test 3: conversation continuity
    auto resp3 = client.chat("What did I just ask you to say?");
    assert(!resp3.text().empty());
    assert(client.conversation().messages.size() == 4); // 2 user + 2 assistant

    println("test_openai_live: ALL PASSED");
    return 0;
}
