// Simple and elegant AI chat CLI tool using streaming
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
            std::print("\nAI: ");
            client.chat_stream(input, [](std::string_view chunk) {
                std::print("{}", chunk);
            });
            std::println("\n");

        } catch (const std::exception& e) {
            std::println("\nError: {}\n", e.what());
        }
    }

    return 0;
}
