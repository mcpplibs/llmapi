# Examples

Practical examples for common use cases.

## C++ Examples

### Hello World

Minimal streaming example:

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;
    
    Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);

    client.model("gpt-5")
          .system("You are a helpful assistant.")
          .user("In one sentence, introduce modern C++.")
          .request([](std::string_view chunk) {
              std::print("{}", chunk);
              std::cout.flush();
          });

    return 0;
}
```

### Chat Application

Interactive CLI chat:

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;
    
    Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
    client.model("gpt-5").system("You are a helpful assistant.");

    std::println("AI Chat CLI - Type 'quit' to exit\n");

    std::string input;
    while (true) {
        std::print("You: ");
        if (!std::getline(std::cin, input) || input == "quit" || input == "q") {
            std::println("\nBye!");
            break;
        }

        if (input.empty()) continue;

        client.user(input);
        std::print("\nAI: ");
        
        client.request([](std::string_view chunk) {
            std::print("{}", chunk);
            std::cout.flush();
        });
        
        std::println("\n");
    }

    return 0;
}
```

### Multi-Turn Conversation

Using conversation history:

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;
    
    Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
    client.model("gpt-5").system("You are a helpful assistant.");

    // First question
    client.user("What is the capital of France?");
    client.request();
    std::println("Q1 Answer: {}\n", client.getAnswer());

    // Follow-up (uses history)
    client.user("What's its population?");
    client.request();
    std::println("Q2 Answer: {}\n", client.getAnswer());

    // Another follow-up
    client.user("Translate the above to Chinese.");
    client.request();
    std::println("Q3 Answer: {}\n", client.getAnswer());

    std::println("Total messages: {}", client.getMessageCount());
    
    return 0;
}
```

### Non-Streaming with JSON Response

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;
    
    Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
    client.model("gpt-5").user("What is 2+2?");

    auto response = client.request();
    
    // Access full JSON response
    std::println("Model: {}", response["model"]);
    std::println("Content: {}", response["choices"][0]["message"]["content"]);
    
    // Or use getAnswer()
    std::println("Answer: {}", client.getAnswer());
    
    return 0;
}
```

### Translation Chain

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;
    
    Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
    client.model("gpt-5");

    // Generate English story
    client.system("You are a creative writer.")
          .user("Write a short story about C++ (50 words)");
    
    std::print("Story: ");
    client.request([](std::string_view chunk) {
        std::print("{}", chunk);
        std::cout.flush();
    });
    std::println("\n");

    // Translate to Chinese (uses history)
    client.user("请把上面的故事翻译成中文。");
    std::print("Translation: ");
    client.request([](std::string_view chunk) {
        std::print("{}", chunk);
        std::cout.flush();
    });
    std::println("");
    
    return 0;
}
```

### Error Handling

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;
    
    try {
        Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
        
        client.model("gpt-5")
              .user("Hello")
              .request();
        
        std::println("{}", client.getAnswer());
        
    } catch (const std::runtime_error& e) {
        std::println("Runtime error: {}", e.what());
    } catch (const std::exception& e) {
        std::println("Error: {}", e.what());
    }
    
    return 0;
}
```

## See Also

- [C++ API Reference](cpp-api.md)
- [Providers Configuration](providers.md)
