# C API Reference

Complete reference for the C API.

## Header

```c
#include "llmapi.h"
```

## Types

### llmapi_client_t

Opaque client handle with function pointer vtable (C-style OOP).

```c
typedef struct llmapi_client_t llmapi_client_t;
```

### llmapi_string_t

String returned by library that **must** be freed with `llmapi_free_string()`.

```c
typedef char* llmapi_string_t;
```

### llmapi_error_t

Error codes.

```c
typedef enum {
    LLMAPI_OK = 0,
    LLMAPI_ERROR_INVALID_PARAM = -1,
    LLMAPI_ERROR_NETWORK = -2,
    LLMAPI_ERROR_PARSE = -3,
    LLMAPI_ERROR_STATE = -4,
    LLMAPI_ERROR_UNKNOWN = -99
} llmapi_error_t;
```

### llmapi_stream_callback_t

Callback function for streaming responses.

```c
typedef void (*llmapi_stream_callback_t)(
    const char* content, 
    size_t length, 
    void* user_data
);
```

## Constants

```c
#define LLMAPI_URL_OPENAI "https://api.openai.com/v1"
#define LLMAPI_URL_POE "https://api.poe.com/v1"
```

## Functions

### llmapi_client_create()

```c
llmapi_client_t* llmapi_client_create(const char* api_key, const char* base_url);
```

Create a new client.

**Parameters:**
- `api_key` - API key (required)
- `base_url` - Base URL (can be NULL for OpenAI default)

**Returns:** Client pointer or NULL on failure

**Example:**
```c
llmapi_client_t* c = llmapi_client_create(getenv("OPENAI_API_KEY"), LLMAPI_URL_POE);
if (!c) {
    fprintf(stderr, "Failed to create client\n");
    return 1;
}
```

### llmapi_free_string()

```c
void llmapi_free_string(llmapi_string_t str);
```

Free a string returned by the library.

**Example:**
```c
llmapi_string_t answer = c->get_answer(c);
printf("%s\n", answer);
llmapi_free_string(answer);  // Must free!
```

## Client Methods

All methods are accessed via function pointers in the `llmapi_client_t` structure.

### Usage Pattern

```c
// Direct vtable call
client->method(client, args...);

// Or use wrapper functions
llmapi_client_method(client, args...);
```

### destroy()

```c
void (*destroy)(llmapi_client_t* self);
// Wrapper: void llmapi_client_destroy(llmapi_client_t* client);
```

Destroy the client and free resources.

**Example:**
```c
c->destroy(c);
// or
llmapi_client_destroy(c);
```

### set_model()

```c
llmapi_error_t (*set_model)(llmapi_client_t* self, const char* model);
// Wrapper: llmapi_error_t llmapi_client_set_model(llmapi_client_t* client, const char* model);
```

Set the model name.

**Example:**
```c
c->set_model(c, "gpt-5");
```

### add_user_message()

```c
llmapi_error_t (*add_user_message)(llmapi_client_t* self, const char* content);
// Wrapper: llmapi_error_t llmapi_client_add_user_message(llmapi_client_t* client, const char* content);
```

Add a user message.

**Example:**
```c
c->add_user_message(c, "What is C++?");
```

### add_system_message()

```c
llmapi_error_t (*add_system_message)(llmapi_client_t* self, const char* content);
// Wrapper: llmapi_error_t llmapi_client_add_system_message(llmapi_client_t* client, const char* content);
```

Add a system message.

**Example:**
```c
c->add_system_message(c, "You are a helpful assistant.");
```

### add_assistant_message()

```c
llmapi_error_t (*add_assistant_message)(llmapi_client_t* self, const char* content);
// Wrapper: llmapi_error_t llmapi_client_add_assistant_message(llmapi_client_t* client, const char* content);
```

Add an assistant message.

**Note:** Usually not needed - responses are auto-saved.

### clear()

```c
llmapi_error_t (*clear)(llmapi_client_t* self);
// Wrapper: llmapi_error_t llmapi_client_clear(llmapi_client_t* client);
```

Clear conversation history.

**Example:**
```c
c->clear(c);
```

### get_message_count()

```c
int (*get_message_count)(llmapi_client_t* self);
// Wrapper: int llmapi_client_get_message_count(llmapi_client_t* client);
```

Get number of messages in history.

**Returns:** Message count or -1 on error

**Example:**
```c
int count = c->get_message_count(c);
printf("Messages: %d\n", count);
```

### get_answer()

```c
llmapi_string_t (*get_answer)(llmapi_client_t* self);
// Wrapper: llmapi_string_t llmapi_client_get_answer(llmapi_client_t* client);
```

Get last assistant reply. **Must free with `llmapi_free_string()`.**

**Returns:** String or NULL if no answer

**Example:**
```c
llmapi_string_t answer = c->get_answer(c);
if (answer) {
    printf("Answer: %s\n", answer);
    llmapi_free_string(answer);
}
```

### request()

```c
llmapi_string_t (*request)(llmapi_client_t* self);
// Wrapper: llmapi_string_t llmapi_client_request(llmapi_client_t* client);
```

Execute non-streaming request. **Auto-saves assistant reply to history.** Returns the response content.

**Returns:** Response string (must free) or NULL on error

**Example:**
```c
c->add_user_message(c, "Hello");
llmapi_string_t response = c->request(c);
if (response) {
    printf("%s\n", response);
    llmapi_free_string(response);
} else {
    fprintf(stderr, "Error: %s\n", c->get_last_error(c));
}
```

### request_stream()

```c
llmapi_error_t (*request_stream)(
    llmapi_client_t* self,
    llmapi_stream_callback_t callback,
    void* user_data
);
// Wrapper: llmapi_error_t llmapi_client_request_stream(...)
```

Execute streaming request. **Auto-saves complete assistant reply to history.**

**Parameters:**
- `callback` - Function called for each content chunk
- `user_data` - User data passed to callback

**Returns:** `LLMAPI_OK` or error code

**Example:**
```c
void print_chunk(const char* s, size_t len, void* data) {
    printf("%.*s", (int)len, s);
    fflush(stdout);
}

c->add_user_message(c, "Tell a story");
c->request_stream(c, print_chunk, NULL);
```

### get_last_error()

```c
const char* (*get_last_error)(llmapi_client_t* self);
// Wrapper: const char* llmapi_get_last_error(llmapi_client_t* client);
```

Get last error message.

**Returns:** Error string or NULL if no error

**Example:**
```c
if (c->request(c) == NULL) {
    fprintf(stderr, "Error: %s\n", c->get_last_error(c));
}
```

## Complete Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llmapi.h"

void stream_print(const char* content, size_t length, void* user_data) {
    printf("%.*s", (int)length, content);
    fflush(stdout);
}

int main(void) {
    // Create client
    llmapi_client_t* c = llmapi_client_create(
        getenv("OPENAI_API_KEY"), 
        LLMAPI_URL_POE
    );
    if (!c) {
        fprintf(stderr, "Failed to create client\n");
        return 1;
    }

    // Configure
    c->set_model(c, "gpt-5");
    c->add_system_message(c, "You are a helpful assistant.");

    // Non-streaming request
    printf("Question 1: What is C++?\n");
    c->add_user_message(c, "What is C++?");
    c->request(c);
    
    llmapi_string_t answer = c->get_answer(c);
    if (answer) {
        printf("Answer: %s\n\n", answer);
        llmapi_free_string(answer);
    }

    // Streaming request (uses conversation history)
    printf("Question 2: Tell me more\n");
    c->add_user_message(c, "Tell me more");
    printf("Answer: ");
    c->request_stream(c, stream_print, NULL);
    printf("\n\n");

    // Check history
    printf("Total messages: %d\n", c->get_message_count(c));

    // Cleanup
    c->destroy(c);
    return 0;
}
```

## Error Handling

```c
// Check function return values
if (c->set_model(c, "gpt-5") != LLMAPI_OK) {
    fprintf(stderr, "Error: %s\n", c->get_last_error(c));
}

// Check NULL returns
llmapi_string_t response = c->request(c);
if (response == NULL) {
    fprintf(stderr, "Error: %s\n", c->get_last_error(c));
} else {
    printf("%s\n", response);
    llmapi_free_string(response);
}
```

## Memory Management

**Rules:**
1. Always call `c->destroy(c)` when done
2. Always free `llmapi_string_t` with `llmapi_free_string()`
3. Don't free strings returned by `get_last_error()` - they're managed internally

**Example:**
```c
llmapi_string_t answer = c->get_answer(c);  // Allocates
if (answer) {
    use_string(answer);
    llmapi_free_string(answer);  // Must free
}

const char* err = c->get_last_error(c);  // No allocation
// Don't free err!
```
