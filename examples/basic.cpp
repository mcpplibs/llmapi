// Basic usage example - demonstrates both streaming and non-streaming modes
import mcpplibs.llmapi;
import std;

#include "print.hpp"

using namespace mcpplibs::llmapi;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        println("Error: OPENAI_API_KEY not set");
        return 1;
    }

    auto client = Client(Config{
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    });
    client.system("You are a helpful assistant.");

    println("=== llmapi Basic Usage Demo ===");
    println();

    try {
        // Example 1: Non-streaming request
        println("[Example 1] Non-streaming mode:");
        println("Question: What is the capital of China?");
        println();

        auto resp = client.chat("What is the capital of China?");
        println("Answer: ", resp.text());
        println();

        // Example 2: Streaming request
        println("[Example 2] Streaming mode:");
        println("Question: Convince me to use modern C++ (100 words)");
        println();

        client.clear();
        client.system("You are a helpful assistant.");
        print("Answer: ");
        auto resp2 = client.chat_stream("Convince me to use modern C++ (100 words)",
            [](std::string_view chunk) {
                print(chunk);
            });
        println();
        println();
        println("[Verification] Answer length: ", resp2.text().size(), " chars");
        println();

    } catch (const std::exception& e) {
        println();
        println("Error: ", e.what());
        println();
        return 1;
    }

    println("=== Demo Complete ===");
    return 0;
}
