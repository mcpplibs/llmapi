import mcpplibs.llmapi;
import std;

#include <cassert>
#include "../test_print.hpp"

using namespace mcpplibs::llmapi;

Task<int> compute_async(int a, int b) {
    co_return a + b;
}

Task<std::string> chain_async() {
    auto result = co_await compute_async(2, 3);
    co_return "result=" + std::to_string(result);
}

Task<void> void_task() {
    co_return;
}

Task<int> throwing_task() {
    throw std::runtime_error("test error");
    co_return 0;
}

// Mock Provider
struct MockProvider {
    std::string_view name() const { return "mock"; }

    ChatResponse chat(const std::vector<Message>&, const ChatParams&) {
        return ChatResponse {
            .content = { TextContent { "mock response" } },
            .stopReason = StopReason::EndOfTurn,
        };
    }

    Task<ChatResponse> chat_async(const std::vector<Message>&, const ChatParams&) {
        co_return ChatResponse {
            .content = { TextContent { "mock async" } },
            .stopReason = StopReason::EndOfTurn,
        };
    }
};

static_assert(Provider<MockProvider>);

int main() {
    // Test 1: Task<int> sync get
    auto t1 = compute_async(3, 4);
    assert(t1.get() == 7);

    // Test 2: Task<string> with co_await chain
    auto t2 = chain_async();
    assert(t2.get() == "result=5");

    // Test 3: Task<void>
    auto t3 = void_task();
    t3.get();

    // Test 4: exception propagation
    auto t4 = throwing_task();
    try {
        t4.get();
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "test error");
    }

    // Test 5: MockProvider satisfies concept
    MockProvider mock;
    auto resp = mock.chat({}, {});
    assert(resp.text() == "mock response");

    auto asyncResp = mock.chat_async({}, {});
    assert(asyncResp.get().text() == "mock async");

    println("test_coro: ALL PASSED");
    return 0;
}
