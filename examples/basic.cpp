// Basic usage example - demonstrates both streaming and non-streaming modes
import std;
import mcpplibs.llmapi;

using namespace mcpplibs;

int main() {
    auto api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        std::println("Error: OPENAI_API_KEY not set");
        return 1;
    }

    llmapi::Client client(api_key, llmapi::URL::Poe);
    client.model("gpt-5").system("You are a helpful assistant.");

    std::println("=== llmapi Basic Usage Demo ===\n");

    try {
        // Example 1: Non-streaming request
        std::println("[Example 1] Non-streaming mode:");
        std::println("Question: What is the capital of China?\n");
        
        client.user("What is the capital of China?");
        client.request();
        
        std::println("Answer: {}\n", client.getAnswer());

        // Example 2: Streaming request
        std::println("[Example 2] Streaming mode:");
        std::println("Question: Convince me to use modern C++ (100 words)\n");

        client.user("Convince me to use modern C++ (100 words)");
        std::print("Answer: ");

        client.request([](std::string_view chunk) {
            std::print("{}", chunk);
            std::cout.flush();
        });
        
        std::println("\n");

        // Verify auto-save: get the last answer
        auto last_answer = client.getAnswer();
        std::println("[Verification] Last answer length: {} chars\n", last_answer.size());

        // Example 3: Translate the story to Chinese
        std::println("[Example 3] Translation (streaming):");
        std::println("Question: 请把上个回答翻译成中文。\n");
        
        client.user("请把上面的故事翻译成中文。");
        std::print("Answer: ");
        
        client.request([](std::string_view chunk) {
            std::print("{}", chunk);
            std::cout.flush();
        });
        
        std::println("\n");
        
    } catch (const std::exception& e) {
        std::println("\nError: {}\n", e.what());
        return 1;
    }

    std::println("=== Demo Complete ===");
    return 0;
}