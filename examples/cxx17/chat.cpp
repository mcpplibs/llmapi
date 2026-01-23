// Simple and elegant AI chat CLI tool using streaming
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

    std::cout << "AI Chat CLI - Type 'quit' to exit\n" << std::endl;

    while (true) {
        std::cout << "You: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "quit" || input == "q") {
            std::cout << "\nBye!" << std::endl;
            break;
        }

        if (input.empty()) continue;

        try {
            client.user(input);
            std::cout << "\nAI: ";

            client.request([](std::string_view chunk) {
                std::cout << chunk;
                std::cout.flush();
            });

            std::cout << "\n" << std::endl;

        } catch (const std::exception& e) {
            std::cout << "\nError: " << e.what() << std::endl;
        }
    }

    return 0;
}