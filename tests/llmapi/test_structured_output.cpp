import mcpplibs.llmapi;
import mcpplibs.llmapi.nlohmann.json;
import std;

#include <cassert>

using namespace mcpplibs::llmapi;
using Json = nlohmann::json;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::println("OPENAI_API_KEY not set, skipping");
        return 0;
    }

    auto client = Client(openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    }));

    // Test: JSON object mode
    auto resp = client.chat("Generate a JSON object with fields: name (string), age (number)",
        ChatParams {
            .responseFormat = ResponseFormat {
                .type = ResponseFormatType::JsonObject,
            },
        });

    auto json = Json::parse(resp.text());
    assert(json.contains("name"));
    assert(json.contains("age"));
    std::println("JSON output: {}", resp.text());

    std::println("test_structured_output: ALL PASSED");
    return 0;
}
