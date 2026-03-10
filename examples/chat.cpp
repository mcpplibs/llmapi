// Simple and elegant AI chat CLI tool using streaming
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

    println("AI Chat CLI - Type 'quit' to exit");
    println();

    while (true) {
        print("You: ");
        std::string input;
        std::getline(std::cin, input);

        if (input == "quit" || input == "q") {
            println();
            println("Bye!");
            break;
        }

        if (input.empty()) continue;

        try {
            print("\nAI: ");
            client.chat_stream(input, [](std::string_view chunk) {
                print(chunk);
            });
            println();
            println();

        } catch (const std::exception& e) {
            println();
            println("Error: ", e.what());
            println();
        }
    }

    return 0;
}
