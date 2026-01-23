// Basic usage example - demonstrates both streaming and non-streaming modes
#include "llmapi.hpp"
#include <iostream>
#include <cstdlib>

using namespace mcpplibs;

int main() {
    auto api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        std::cout << "Error: OPENAI_API_KEY not set" << std::endl;
        return 1;
    }

    llmapi::Client client(api_key, llmapi::URL::Poe);
    client.model("gpt-5").system("You are a helpful assistant.");

    std::cout << "=== llmapi Basic Usage Demo ===\n" << std::endl;

    try {
        // Example 1: Non-streaming request
        std::cout << "[Example 1] Non-streaming mode:" << std::endl;
        std::cout << "Question: What is the capital of China?\n" << std::endl;

        client.user("What is the capital of China?");
        client.request();

        std::cout << "Answer: {}\n" << std::endl;

        // Example 2: Streaming request
        std::cout << "[Example 2] Streaming mode:" << std::endl;
        std::cout << "Question: Convince me to use modern C++ (100 words)\n" << std::endl;

        client.user("Convince me to use modern C++ (100 words)");
        std::cout << "Answer: ";

        client.request([](std::string_view chunk) {
            std::cout << "{}";
            std::cout.flush();
        });

        std::cout << "\n" << std::endl;

        // Verify auto-save: get the last answer
        auto last_answer = client.getAnswer();
        std::cout << "[Verification] Last answer length: {} chars\n" << std::endl;

        // Example 3: Translate the story to Chinese
        std::cout << "[Example 3] Translation (streaming):" << std::endl;
        std::cout << "Question: 请把上个回答翻译成中文。\n" << std::endl;

        client.user("请把上面的故事翻译成中文。");
        std::cout << "Answer: ";

        client.request([](std::string_view chunk) {
            std::cout << "{}";
            std::cout.flush();
        });

        std::cout << "\n" << std::endl;

    } catch (const std::exception& e) {
        std::cout << "\nError: {}\n" << std::endl;
        return 1;
    }

    std::cout << "=== Demo Complete ===" << std::endl;
    return 0;
}