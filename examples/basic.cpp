// Basic usage example - demonstrates both streaming and non-streaming modes
import mcpplibs.llmapi;
import std;

using namespace mcpplibs::llmapi;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::println("Error: OPENAI_API_KEY not set");
        return 1;
    }

    auto client = Client(openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    }));
    client.system("You are a helpful assistant.");

    std::println("=== llmapi Basic Usage Demo ===\n");

    try {
        // Example 1: Non-streaming request
        std::println("[Example 1] Non-streaming mode:");
        std::println("Question: What is the capital of China?\n");

        auto resp = client.chat("What is the capital of China?");
        std::println("Answer: {}\n", resp.text());

        // Example 2: Streaming request
        std::println("[Example 2] Streaming mode:");
        std::println("Question: Convince me to use modern C++ (100 words)\n");

        client.clear();
        client.system("You are a helpful assistant.");
        std::print("Answer: ");
        auto resp2 = client.chat_stream("Convince me to use modern C++ (100 words)",
            [](std::string_view chunk) {
                std::print("{}", chunk);
            });
        std::println("\n");
        std::println("[Verification] Answer length: {} chars\n", resp2.text().size());

    } catch (const std::exception& e) {
        std::println("\nError: {}\n", e.what());
        return 1;
    }

    std::println("=== Demo Complete ===");
    return 0;
}
