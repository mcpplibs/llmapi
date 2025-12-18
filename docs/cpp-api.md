# C++ API Reference

Complete reference for the C++ API.

## Namespace

```cpp
import mcpplibs.llmapi;
using namespace mcpplibs::llmapi;
```

## Client Class

### Constructor

```cpp
Client(std::string_view apiKey, std::string_view baseUrl = URL::OpenAI)
Client(const char* apiKey, std::string_view baseUrl = URL::OpenAI)
```

**Parameters:**
- `apiKey` - API key (can be from `std::getenv()`)
- `baseUrl` - Base URL (see [Providers](providers.md))

**Example:**
```cpp
Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
```

### Configuration Methods

All configuration methods return `Client&` for chaining.

#### model()

```cpp
Client& model(std::string_view model)
```

Set the model name.

**Example:**
```cpp
client.model("gpt-5");
```

### Message Methods

All message methods return `Client&` for chaining.

#### user()

```cpp
Client& user(std::string_view content)
```

Add a user message.

**Example:**
```cpp
client.user("What is C++?");
```

#### system()

```cpp
Client& system(std::string_view content)
```

Add a system message (usually for initial instructions).

**Example:**
```cpp
client.system("You are a helpful assistant.");
```

#### assistant()

```cpp
Client& assistant(std::string_view content)
```

Add an assistant message (usually for few-shot examples or manual history).

**Note:** Normally not needed - responses are auto-saved to history.

**Example:**
```cpp
client.assistant("I understand.");
```

#### add_message()

```cpp
Client& add_message(std::string_view role, std::string_view content)
```

Add a message with custom role.

**Example:**
```cpp
client.add_message("user", "Hello");
```

#### clear()

```cpp
Client& clear()
```

Clear all conversation history.

**Example:**
```cpp
client.clear();
```

### Request Methods

#### request() - Non-Streaming

```cpp
Json request()
```

Execute a non-streaming request. Returns full JSON response. **Automatically saves assistant reply to history.**

**Returns:** `nlohmann::json` object with full API response

**Example:**
```cpp
auto response = client.user("Hello").request();
std::println("{}", response["choices"][0]["message"]["content"]);
```

#### request(callback) - Streaming

```cpp
template<StreamCallback Callback>
void request(Callback&& callback)
```

Execute a streaming request. **Automatically saves complete assistant reply to history.**

**Parameters:**
- `callback` - Function accepting `std::string_view` (each content chunk)

**Example:**
```cpp
client.user("Tell me a story").request([](std::string_view chunk) {
    std::print("{}", chunk);
    std::cout.flush();
});
```

### Getter Methods

#### getAnswer()

```cpp
std::string getAnswer() const
```

Get the last assistant reply from conversation history.

**Returns:** Last assistant message content, or empty string if none

**Example:**
```cpp
client.request();
std::string answer = client.getAnswer();
std::println("Last answer: {}", answer);
```

#### getMessages()

```cpp
Json getMessages() const
```

Get full conversation history as JSON array.

**Returns:** JSON array of all messages

**Example:**
```cpp
auto history = client.getMessages();
for (const auto& msg : history) {
    std::println("{}: {}", msg["role"], msg["content"]);
}
```

#### getMessageCount()

```cpp
int getMessageCount() const
```

Get total number of messages in conversation history.

**Returns:** Number of messages

**Example:**
```cpp
std::println("Messages: {}", client.getMessageCount());
```

#### getApiKey()

```cpp
std::string_view getApiKey() const
```

Get the API key.

#### getBaseUrl()

```cpp
std::string_view getBaseUrl() const
```

Get the base URL.

#### getModel()

```cpp
std::string_view getModel() const
```

Get the current model name.

## StreamCallback Concept

```cpp
template<typename F>
concept StreamCallback = std::invocable<F, std::string_view> && 
                        std::same_as<std::invoke_result_t<F, std::string_view>, void>;
```

Type constraint for streaming callbacks. Accepts any callable that:
- Takes `std::string_view` parameter
- Returns `void`

**Valid callbacks:**
```cpp
// Lambda
[](std::string_view chunk) { std::print("{}", chunk); }

// Function
void my_callback(std::string_view chunk) { /* ... */ }

// Functor
struct Printer {
    void operator()(std::string_view chunk) { /* ... */ }
};
```

## JSON Type

```cpp
using Json = nlohmann::json;
```

The library uses [nlohmann/json](https://github.com/nlohmann/json) for JSON handling.

## Complete Example

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;
    
    Client client(std::getenv("OPENAI_API_KEY"), URL::Poe);
    
    // Configure
    client.model("gpt-5")
          .system("You are a helpful assistant.");
    
    // First question (non-streaming)
    client.user("What is C++?");
    client.request();
    std::println("Answer 1: {}", client.getAnswer());
    
    // Follow-up (streaming) - uses conversation history
    client.user("Tell me more");
    std::print("Answer 2: ");
    client.request([](std::string_view chunk) {
        std::print("{}", chunk);
        std::cout.flush();
    });
    std::println("\n");
    
    // Check history
    std::println("Total messages: {}", client.getMessageCount());
    
    return 0;
}
```

## Error Handling

All methods may throw exceptions:
- `std::runtime_error` - API errors, network errors
- `std::invalid_argument` - Invalid parameters
- `nlohmann::json::exception` - JSON parsing errors

**Example:**
```cpp
try {
    client.user("Hello").request();
} catch (const std::runtime_error& e) {
    std::println("Error: {}", e.what());
}
```
