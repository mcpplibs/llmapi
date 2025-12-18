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

## C Examples

### Hello World

```c
#include <stdio.h>
#include <stdlib.h>
#include "llmapi.h"

void print_chunk(const char* s, size_t len, void* data) {
    printf("%.*s", (int)len, s);
    fflush(stdout);
}

int main(void) {
    llmapi_client_t* c = llmapi_client_create(
        getenv("OPENAI_API_KEY"), 
        LLMAPI_URL_POE
    );

    c->set_model(c, "gpt-5");
    c->add_system_message(c, "You are a helpful assistant.");
    c->add_user_message(c, "In one sentence, introduce modern C++.");
    c->request_stream(c, print_chunk, NULL);
    
    c->destroy(c);
    return 0;
}
```

### Chat Application

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llmapi.h"

void stream_print(const char* s, size_t len, void* data) {
    printf("%.*s", (int)len, s);
    fflush(stdout);
}

int main(void) {
    llmapi_client_t* c = llmapi_client_create(
        getenv("OPENAI_API_KEY"), 
        LLMAPI_URL_POE
    );
    if (!c) {
        fprintf(stderr, "Failed to create client\n");
        return 1;
    }

    c->set_model(c, "gpt-5");
    c->add_system_message(c, "You are a helpful assistant.");

    printf("AI Chat CLI (C) - Type 'quit' to exit\n\n");

    char input[4096];
    while (1) {
        printf("You: ");
        if (!fgets(input, sizeof(input), stdin)) break;

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') input[len - 1] = '\0';

        if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
            printf("\nBye!\n");
            break;
        }

        if (strlen(input) == 0) continue;

        c->add_user_message(c, input);
        printf("\nAI: ");
        c->request_stream(c, stream_print, NULL);
        printf("\n\n");
    }

    c->destroy(c);
    return 0;
}
```

### Non-Streaming Request

```c
#include <stdio.h>
#include <stdlib.h>
#include "llmapi.h"

int main(void) {
    llmapi_client_t* c = llmapi_client_create(
        getenv("OPENAI_API_KEY"), 
        LLMAPI_URL_POE
    );
    if (!c) return 1;

    c->set_model(c, "gpt-5");
    c->add_system_message(c, "You are a helpful assistant.");
    c->add_user_message(c, "What is C++?");
    
    // Get response
    c->request(c);
    
    // Get saved answer
    llmapi_string_t answer = c->get_answer(c);
    if (answer) {
        printf("Answer: %s\n", answer);
        llmapi_free_string(answer);
    } else {
        fprintf(stderr, "Error: %s\n", c->get_last_error(c));
    }

    c->destroy(c);
    return 0;
}
```

### Multi-Turn Conversation

```c
#include <stdio.h>
#include <stdlib.h>
#include "llmapi.h"

int main(void) {
    llmapi_client_t* c = llmapi_client_create(
        getenv("OPENAI_API_KEY"), 
        LLMAPI_URL_POE
    );

    c->set_model(c, "gpt-5");
    c->add_system_message(c, "You are a helpful assistant.");

    // Question 1
    c->add_user_message(c, "What is the capital of France?");
    c->request(c);
    
    llmapi_string_t ans1 = c->get_answer(c);
    printf("Q1: %s\n\n", ans1);
    llmapi_free_string(ans1);

    // Question 2 (uses history)
    c->add_user_message(c, "What's its population?");
    c->request(c);
    
    llmapi_string_t ans2 = c->get_answer(c);
    printf("Q2: %s\n\n", ans2);
    llmapi_free_string(ans2);

    printf("Total messages: %d\n", c->get_message_count(c));

    c->destroy(c);
    return 0;
}
```

### Error Handling

```c
#include <stdio.h>
#include <stdlib.h>
#include "llmapi.h"

int main(void) {
    llmapi_client_t* c = llmapi_client_create(
        getenv("OPENAI_API_KEY"), 
        LLMAPI_URL_POE
    );
    if (!c) {
        fprintf(stderr, "Failed to create client\n");
        return 1;
    }

    // Check each operation
    if (c->set_model(c, "gpt-5") != LLMAPI_OK) {
        fprintf(stderr, "set_model error: %s\n", c->get_last_error(c));
        c->destroy(c);
        return 1;
    }

    c->add_user_message(c, "Hello");
    
    llmapi_string_t response = c->request(c);
    if (!response) {
        fprintf(stderr, "request error: %s\n", c->get_last_error(c));
        c->destroy(c);
        return 1;
    }

    printf("Response: %s\n", response);
    llmapi_free_string(response);

    c->destroy(c);
    return 0;
}
```

### Custom User Data in Streaming

```c
#include <stdio.h>
#include <stdlib.h>
#include "llmapi.h"

typedef struct {
    int chunk_count;
    size_t total_size;
} StreamStats;

void counting_callback(const char* s, size_t len, void* data) {
    StreamStats* stats = (StreamStats*)data;
    stats->chunk_count++;
    stats->total_size += len;
    printf("%.*s", (int)len, s);
    fflush(stdout);
}

int main(void) {
    llmapi_client_t* c = llmapi_client_create(
        getenv("OPENAI_API_KEY"), 
        LLMAPI_URL_POE
    );

    c->set_model(c, "gpt-5");
    c->add_user_message(c, "Write a short poem");
    
    StreamStats stats = {0, 0};
    c->request_stream(c, counting_callback, &stats);
    
    printf("\n\nStats: %d chunks, %zu bytes\n", 
           stats.chunk_count, stats.total_size);

    c->destroy(c);
    return 0;
}
```

## See Also

- [C++ API Reference](cpp-api.md)
- [C API Reference](c-api.md)
- [Providers Configuration](providers.md)
