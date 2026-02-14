# Advanced Usage

Advanced features and patterns.

## Conversation History Management

### Auto-Save Behavior

Responses are automatically saved to conversation history:

```cpp
client.user("Question 1").request();
// Assistant reply auto-saved ✅

client.user("Question 2").request([](auto chunk) { 
    std::print("{}", chunk); 
});
// Streamed reply auto-saved ✅

client.user("Question 3");  // Can reference previous context
```

### Manual History Manipulation

```cpp
// Add few-shot examples
client.system("You are a translator.")
      .user("Hello")
      .assistant("你好")
      .user("Goodbye")
      .assistant("再见")
      .user("Thank you");  // Will use examples

// Clear history
client.clear();  // Start fresh conversation
```

### Get Conversation History

```cpp
// Get last answer
std::string answer = client.getAnswer();

// Get all messages
auto messages = client.getMessages();
for (const auto& msg : messages) {
    std::println("{}: {}", msg["role"], msg["content"]);
}

// Get message count
int count = client.getMessageCount();
std::println("Total: {} messages", count);
```

## Streaming Patterns

### Basic Streaming

```cpp
client.user("Tell a story").request([](std::string_view chunk) {
    std::print("{}", chunk);
    std::cout.flush();
});
```

### Collecting Stream Data

```cpp
std::string full_response;
client.user("Question").request([&](std::string_view chunk) {
    full_response += chunk;
    std::print("{}", chunk);
    std::cout.flush();
});

// After streaming, can use full_response
// Or just use client.getAnswer()
```

### Stream with Statistics

```cpp
struct Stats {
    int chunks = 0;
    size_t bytes = 0;
    auto start = std::chrono::steady_clock::now();
};

Stats stats;
client.user("Generate text").request([&](std::string_view chunk) {
    stats.chunks++;
    stats.bytes += chunk.size();
    std::print("{}", chunk);
});

auto duration = std::chrono::steady_clock::now() - stats.start;
std::println("\nStats: {} chunks, {} bytes in {}ms",
    stats.chunks, 
    stats.bytes,
    std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
);
```

### Conditional Stream Processing

```cpp
bool first_chunk = true;
client.user("Question").request([&](std::string_view chunk) {
    if (first_chunk) {
        std::print("Answer: ");
        first_chunk = false;
    }
    std::print("{}", chunk);
    std::cout.flush();
});
std::println("");
```

## Non-Streaming with JSON

### Access Full Response

```cpp
auto response = client.user("Question").request();

// Model info
std::println("Model: {}", response["model"]);

// Usage stats
auto usage = response["usage"];
std::println("Tokens - prompt: {}, completion: {}, total: {}",
    usage["prompt_tokens"],
    usage["completion_tokens"],
    usage["total_tokens"]
);

// Content (or use getAnswer())
std::string content = response["choices"][0]["message"]["content"];
```

### Save Full Response

```cpp
auto response = client.user("Question").request();

// Save to file
std::ofstream file("response.json");
file << response.dump(2);  // Pretty print with 2-space indent
```

## Error Handling Patterns

### Try-Catch Pattern

```cpp
try {
    client.user("Question").request();
    std::println("{}", client.getAnswer());
    
} catch (const std::runtime_error& e) {
    std::println("Runtime error: {}", e.what());
} catch (const std::invalid_argument& e) {
    std::println("Invalid argument: {}", e.what());
} catch (const std::exception& e) {
    std::println("Error: {}", e.what());
}
```

### Graceful Degradation

```cpp
std::optional<std::string> safe_request(Client& client, std::string_view query) {
    try {
        client.user(query).request();
        return client.getAnswer();
    } catch (...) {
        return std::nullopt;
    }
}

// Usage
if (auto answer = safe_request(client, "Question")) {
    std::println("{}", *answer);
} else {
    std::println("Request failed, using fallback");
}
```

### Retry Logic

```cpp
std::string request_with_retry(Client& client, std::string_view query, int max_retries = 3) {
    for (int i = 0; i < max_retries; i++) {
        try {
            client.user(query).request();
            return client.getAnswer();
        } catch (const std::exception& e) {
            if (i == max_retries - 1) throw;
            std::println("Retry {}/{}", i + 1, max_retries);
            std::this_thread::sleep_for(std::chrono::seconds(1 << i));  // Exponential backoff
        }
    }
    throw std::runtime_error("Max retries exceeded");
}
```

## Multi-Client Pattern

```cpp
// Different clients for different purposes
Client translator(api_key, URL::Poe);
translator.model("gpt-5").system("You are a translator.");

Client summarizer(api_key, URL::Poe);
summarizer.model("gpt-5").system("You are a text summarizer.");

Client coder(api_key, URL::Poe);
coder.model("gpt-5").system("You are a coding assistant.");

// Use each for specific tasks
translator.user("Translate 'hello' to Chinese").request();
summarizer.user("Summarize this article...").request();
coder.user("Write a function to...").request();
```

## Session Management

```cpp
struct Session {
    Client client;
    std::string session_id;
    std::chrono::steady_clock::time_point created_at;
    
    Session(std::string_view key, std::string_view url) 
        : client(key, url),
          session_id(generate_uuid()),
          created_at(std::chrono::steady_clock::now()) {}
    
    void save_history(const std::filesystem::path& path) {
        std::ofstream file(path);
        file << client.getMessages().dump(2);
    }
};
```

## Callback Patterns

### Lambda Capture

```cpp
std::string buffer;
int line_count = 0;

client.user("Generate list").request([&](std::string_view chunk) {
    buffer += chunk;
    if (chunk.find('\n') != std::string_view::npos) {
        line_count++;
    }
    std::print("{}", chunk);
});

std::println("\nGenerated {} lines", line_count);
```

### Function Objects

```cpp
struct StreamPrinter {
    std::ostream& out;
    
    StreamPrinter(std::ostream& o) : out(o) {}
    
    void operator()(std::string_view chunk) {
        out << chunk;
        out.flush();
    }
};

// Usage
StreamPrinter printer(std::cout);
client.user("Question").request(printer);

// Or to file
std::ofstream file("output.txt");
StreamPrinter file_printer(file);
client.user("Question").request(file_printer);
```

## See Also

- [C++ API Reference](cpp-api.md)
- [Examples](examples.md)
