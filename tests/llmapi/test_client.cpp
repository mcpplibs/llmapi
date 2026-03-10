import mcpplibs.llmapi;
import std;

#include <cassert>
#include "../test_print.hpp"

using namespace mcpplibs::llmapi;

struct FullMockProvider {
    std::string_view name() const { return "full_mock"; }

    ChatResponse chat(const std::vector<Message>& msgs, const ChatParams&) {
        std::string lastContent;
        if (!msgs.empty()) {
            auto& c = msgs.back().content;
            if (auto* s = std::get_if<std::string>(&c)) {
                lastContent = *s;
            }
        }
        return ChatResponse {
            .content = { TextContent { "reply to: " + lastContent } },
            .stopReason = StopReason::EndOfTurn,
            .usage = { .inputTokens = 10, .outputTokens = 5, .totalTokens = 15 },
        };
    }

    Task<ChatResponse> chat_async(const std::vector<Message>& msgs, const ChatParams& p) {
        co_return chat(msgs, p);
    }

    ChatResponse chat_stream(const std::vector<Message>& msgs,
                             const ChatParams& params,
                             std::function<void(std::string_view)> callback) {
        auto resp = chat(msgs, params);
        auto text = resp.text();
        for (std::size_t i = 0; i < text.size(); i += 5) {
            auto len = std::min(std::size_t{5}, text.size() - i);
            callback(std::string_view(text).substr(i, len));
        }
        return resp;
    }

    Task<ChatResponse> chat_stream_async(const std::vector<Message>& msgs,
                                          const ChatParams& params,
                                          std::function<void(std::string_view)> callback) {
        co_return chat_stream(msgs, params, std::move(callback));
    }
};

static_assert(Provider<FullMockProvider>);
static_assert(StreamableProvider<FullMockProvider>);

int main() {
    auto client = Client(FullMockProvider{});

    // Test 1: basic chat
    auto resp = client.chat("hello");
    assert(resp.text() == "reply to: hello");

    // Test 2: conversation auto-saved
    assert(client.conversation().size() == 2);

    // Test 3: system message
    client.clear();
    client.system("be helpful");
    auto resp2 = client.chat("hi");
    assert(client.conversation().size() == 3);

    // Test 4: streaming
    client.clear();
    std::string streamed;
    auto resp3 = client.chat_stream("test", [&streamed](std::string_view chunk) {
        streamed += chunk;
    });
    assert(!streamed.empty());
    assert(resp3.text() == streamed);

    // Test 5: async chat
    client.clear();
    auto asyncResp = client.chat_async("async hello");
    auto result = asyncResp.get();
    assert(result.text() == "reply to: async hello");

    // Test 6: default params
    client.clear();
    client.default_params(ChatParams { .temperature = 0.5 });
    auto resp4 = client.chat("with params");
    assert(resp4.text().find("with params") != std::string::npos);

    // Test 7: save/load conversation
    client.clear();
    client.chat("save me");
    client.save_conversation("/tmp/test_client_conv.json");
    auto client2 = Client(FullMockProvider{});
    client2.load_conversation("/tmp/test_client_conv.json");
    assert(client2.conversation().size() == 2);
    std::filesystem::remove("/tmp/test_client_conv.json");

    println("test_client: ALL PASSED");
    return 0;
}
