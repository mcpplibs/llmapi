#ifndef LLMAPI_H__MCPPLIBS__
#define LLMAPI_H__MCPPLIBS__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

// Forward declaration
typedef struct llmapi_client_t llmapi_client_t;

// Error codes
typedef enum {
    LLMAPI_OK = 0,
    LLMAPI_ERROR_INVALID_PARAM = -1,
    LLMAPI_ERROR_NETWORK = -2,
    LLMAPI_ERROR_PARSE = -3,
    LLMAPI_ERROR_STATE = -4,
    LLMAPI_ERROR_UNKNOWN = -99
} llmapi_error_t;

// Stream callback function type
typedef void (*llmapi_stream_callback_t)(const char* content, size_t length, void* user_data);

// String type that must be freed with llmapi_free_string()
// This is a semantic marker to indicate ownership transfer
typedef char* llmapi_string_t;

// Member function pointer types
typedef void (*llmapi_destroy_fn)(llmapi_client_t* self);
typedef llmapi_error_t (*llmapi_set_model_fn)(llmapi_client_t* self, const char* model);
typedef llmapi_error_t (*llmapi_add_message_fn)(llmapi_client_t* self, const char* content);
typedef llmapi_error_t (*llmapi_clear_fn)(llmapi_client_t* self);
typedef int (*llmapi_get_message_count_fn)(llmapi_client_t* self);
typedef llmapi_string_t (*llmapi_get_answer_fn)(llmapi_client_t* self);
typedef llmapi_string_t (*llmapi_request_fn)(llmapi_client_t* self);
typedef llmapi_error_t (*llmapi_request_stream_fn)(llmapi_client_t* self, 
    llmapi_stream_callback_t callback, void* user_data);
typedef const char* (*llmapi_get_last_error_fn)(llmapi_client_t* self);

// C-style OOP client structure with virtual function table
struct llmapi_client_t {
    // Virtual function table (method pointers)
    llmapi_destroy_fn destroy;
    llmapi_set_model_fn set_model;
    llmapi_add_message_fn add_user_message;
    llmapi_add_message_fn add_system_message;
    llmapi_add_message_fn add_assistant_message;
    llmapi_clear_fn clear;
    llmapi_get_message_count_fn get_message_count;
    llmapi_get_answer_fn get_answer;
    llmapi_request_fn request;
    llmapi_request_stream_fn request_stream;
    llmapi_get_last_error_fn get_last_error;
    
    // Private data (opaque pointer to implementation)
    void* _private;
};

// Constructor - Create a new Client
// Returns NULL on failure
llmapi_client_t* llmapi_client_create(const char* api_key, const char* base_url);

// Convenience wrapper functions (optional - can use client->method(client, ...) directly)
static inline void llmapi_client_destroy(llmapi_client_t* client) {
    if (client && client->destroy) client->destroy(client);
}

static inline llmapi_error_t llmapi_client_set_model(llmapi_client_t* client, const char* model) {
    return client ? client->set_model(client, model) : LLMAPI_ERROR_INVALID_PARAM;
}

static inline llmapi_error_t llmapi_client_add_user_message(llmapi_client_t* client, const char* content) {
    return client ? client->add_user_message(client, content) : LLMAPI_ERROR_INVALID_PARAM;
}

static inline llmapi_error_t llmapi_client_add_system_message(llmapi_client_t* client, const char* content) {
    return client ? client->add_system_message(client, content) : LLMAPI_ERROR_INVALID_PARAM;
}

static inline llmapi_error_t llmapi_client_add_assistant_message(llmapi_client_t* client, const char* content) {
    return client ? client->add_assistant_message(client, content) : LLMAPI_ERROR_INVALID_PARAM;
}

static inline llmapi_error_t llmapi_client_clear(llmapi_client_t* client) {
    return client ? client->clear(client) : LLMAPI_ERROR_INVALID_PARAM;
}

static inline int llmapi_client_get_message_count(llmapi_client_t* client) {
    return client ? client->get_message_count(client) : -1;
}

static inline llmapi_string_t llmapi_client_get_answer(llmapi_client_t* client) {
    return client ? client->get_answer(client) : NULL;
}

static inline llmapi_string_t llmapi_client_request(llmapi_client_t* client) {
    return client ? client->request(client) : NULL;
}

static inline llmapi_error_t llmapi_client_request_stream(
    llmapi_client_t* client,
    llmapi_stream_callback_t callback,
    void* user_data
) {
    return client ? client->request_stream(client, callback, user_data) : LLMAPI_ERROR_INVALID_PARAM;
}

static inline const char* llmapi_get_last_error(llmapi_client_t* client) {
    return client ? client->get_last_error(client) : "Invalid client";
}

// Free string returned by library
void llmapi_free_string(llmapi_string_t str);

// Predefined constants
#define LLMAPI_URL_OPENAI "https://api.openai.com/v1"
#define LLMAPI_URL_POE "https://api.poe.com/v1"

#ifdef __cplusplus
}
#endif

#endif // LLMAPI_H__MCPPLIBS__
