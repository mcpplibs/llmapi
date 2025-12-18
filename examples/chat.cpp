// Simple and elegant AI chat CLI tool using streaming
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

    std::println("AI Chat CLI - Type 'quit' to exit\n");

    while (true) {
        std::print("You: ");
        std::string input;
        std::getline(std::cin, input);

        if (input == "quit" || input == "q") {
            std::println("\nBye!");
            break;
        }

        if (input.empty()) continue;

        try {
            client.user(input);
            std::print("\nAI: ");
            
            client.request([](std::string_view chunk) {
                std::print("{}", chunk);
                std::cout.flush();
            });
            
            std::println("\n");
            
        } catch (const std::exception& e) {
            std::println("\nError: {}\n", e.what());
        }
    }

    return 0;
}