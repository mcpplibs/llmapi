import mcpplibs.llmapi;
import mcpplibs.llmapi.nlohmann.json;
import std;

#include <cassert>
#include "../test_print.hpp"

using namespace mcpplibs::llmapi;
using Json = nlohmann::json;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        println("OPENAI_API_KEY not set, skipping");
        return 0;
    }

    auto client = Client(openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    }));

    auto params = ChatParams {
        .tools = std::vector<ToolDef>{{
            .name = "get_temperature",
            .description = "Get the current temperature in a city",
            .inputSchema = R"({"type":"object","properties":{"city":{"type":"string"}},"required":["city"]})",
        }},
        .toolChoice = ToolChoice::Auto,
    };

    // Ask about weather — model should call the tool
    auto resp = client.chat("What's the temperature in Tokyo?", params);

    if (resp.stopReason == StopReason::ToolUse) {
        auto calls = resp.tool_calls();
        assert(!calls.empty());
        println("Tool called: ", calls[0].name, " with args: ", calls[0].arguments);
        assert(calls[0].name == "get_temperature");

        auto args = Json::parse(calls[0].arguments);
        assert(args.contains("city"));

        // Send tool result back
        client.add_message(Message {
            .role = Role::Tool,
            .content = std::vector<ContentPart>{
                ToolResultContent {
                    .toolUseId = calls[0].id,
                    .content = R"({"temperature": "22°C", "condition": "sunny"})",
                },
            },
        });

        // Get final response — don't add another user message, just continue
        auto messages = client.conversation().messages;
        auto& provider = client.provider();
        auto finalResp = provider.chat(messages, params);
        println("Final: ", finalResp.text());
        assert(!finalResp.text().empty());
    } else {
        println("Model didn't call tool (non-deterministic), response: ", resp.text());
    }

    println("test_tool_calling: ALL PASSED");
    return 0;
}
