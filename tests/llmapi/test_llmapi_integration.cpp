import mcpplibs.llmapi;
import std;

#include <cassert>
#include "../test_print.hpp"

using namespace mcpplibs::llmapi;

int main() {
    // Test 1: compile-time — both providers satisfy concepts
    static_assert(Provider<openai::OpenAI>);
    static_assert(StreamableProvider<openai::OpenAI>);
    static_assert(EmbeddableProvider<openai::OpenAI>);
    static_assert(Provider<anthropic::Anthropic>);
    static_assert(StreamableProvider<anthropic::Anthropic>);

    // Test 2: type system
    auto msg = Message::user("hello");
    ChatParams params { .temperature = 0.7, .maxTokens = 100 };
    Conversation conv;
    conv.push(msg);
    assert(conv.size() == 1);

    // Test 3: Client compiles with both providers
    auto openaiClient = Client(openai::OpenAI({
        .apiKey = "test",
        .model = "gpt-4o",
    }));
    auto anthropicClient = Client(anthropic::Anthropic({
        .apiKey = "test",
        .model = "claude-sonnet-4-20250514",
    }));

    // Test 4: conversation serialization round-trip via save/load
    Conversation conv2;
    conv2.push(Message::system("You are helpful"));
    conv2.push(Message::user("Hello"));
    conv2.push(Message::assistant("Hi there!"));

    auto tmpPath = std::filesystem::temp_directory_path() / "llmapi_test_integration.json";
    conv2.save(tmpPath.string());
    auto conv3 = Conversation::load(tmpPath.string());
    std::filesystem::remove(tmpPath);
    assert(conv3.size() == 3);
    assert(conv3.messages[0].role == Role::System);
    assert(conv3.messages[1].role == Role::User);
    assert(conv3.messages[2].role == Role::Assistant);

    // Test 5: tool definitions
    ToolDef tool {
        .name = "search",
        .description = "Search the web",
        .inputSchema = R"({"type":"object","properties":{"query":{"type":"string"}}})",
    };
    ChatParams toolParams {
        .tools = std::vector<ToolDef>{tool},
        .toolChoice = ToolChoicePolicy{ToolChoice::Auto},
    };
    assert(toolParams.tools->size() == 1);

    // Test 6: response format
    ChatParams jsonParams {
        .responseFormat = ResponseFormat {
            .type = ResponseFormatType::JsonObject,
        },
    };

    // Test 7: ChatResponse helpers
    ChatResponse resp;
    resp.content.push_back(TextContent { .text = "Hello" });
    resp.content.push_back(ToolUseContent {
        .id = "call_1",
        .name = "search",
        .inputJson = R"({"query":"test"})",
    });
    assert(resp.text() == "Hello");
    assert(resp.tool_calls().size() == 1);
    assert(resp.tool_calls()[0].name == "search");

    println("test_integration: ALL PASSED");
    return 0;
}
