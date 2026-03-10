import mcpplibs.llmapi;
import std;

#include <cassert>
#include "../test_print.hpp"

int main() {
    using namespace mcpplibs::llmapi;

    // Test 1: Message construction
    auto msg = Message::user("hello");
    assert(msg.role == Role::User);
    assert(std::holds_alternative<std::string>(msg.content));
    assert(std::get<std::string>(msg.content) == "hello");

    // Test 2: System/Assistant messages
    auto sys = Message::system("you are helpful");
    assert(sys.role == Role::System);
    auto asst = Message::assistant("hi there");
    assert(asst.role == Role::Assistant);

    // Test 3: Multimodal content
    auto multiMsg = Message{
        .role = Role::User,
        .content = std::vector<ContentPart>{
            TextContent{"describe this"},
            ImageContent{.data = "https://example.com/img.jpg", .isUrl = true},
        },
    };
    auto& parts = std::get<std::vector<ContentPart>>(multiMsg.content);
    assert(parts.size() == 2);
    assert(std::holds_alternative<TextContent>(parts[0]));
    assert(std::holds_alternative<ImageContent>(parts[1]));

    // Test 4: ToolDef
    ToolDef tool{
        .name = "get_weather",
        .description = "Get weather",
        .inputSchema = R"({"type":"object"})",
    };
    assert(tool.name == "get_weather");

    // Test 5: ChatParams with optionals
    ChatParams params{
        .temperature = 0.7,
        .maxTokens = 1024,
    };
    assert(params.temperature.has_value());
    assert(!params.topP.has_value());

    // Test 6: ChatResponse text extraction
    ChatResponse resp{
        .content = {TextContent{"hello"}, TextContent{" world"}},
        .stopReason = StopReason::EndOfTurn,
    };
    assert(resp.text() == "hello world");

    // Test 7: ChatResponse tool_calls extraction
    ChatResponse toolResp{
        .content =
            {
                TextContent{"Let me check"},
                ToolUseContent{.id = "call_1", .name = "weather", .inputJson = "{}"},
            },
    };
    auto calls = toolResp.tool_calls();
    assert(calls.size() == 1);
    assert(calls[0].name == "weather");

    // Test 8: Conversation
    Conversation conv;
    conv.push(Message::user("hi"));
    conv.push(Message::assistant("hello"));
    assert(conv.size() == 2);
    conv.clear();
    assert(conv.size() == 0);

    // Test 9: Usage
    Usage usage{.inputTokens = 10, .outputTokens = 20, .totalTokens = 30};
    assert(usage.totalTokens == 30);

    // Test 10: StopReason enum
    assert(StopReason::EndOfTurn != StopReason::ToolUse);

    println("test_types: ALL PASSED");
    return 0;
}
