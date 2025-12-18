// Basic usage example (C API) - demonstrates both streaming and non-streaming
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llmapi.h"

void stream_print(const char* content, size_t length, void* user_data) {
    printf("%.*s", (int)length, content);
    fflush(stdout);
}

int main(void) {
    const char* api_key = getenv("OPENAI_API_KEY");
    if (!api_key) {
        fprintf(stderr, "Error: OPENAI_API_KEY not set\n");
        return 1;
    }

    llmapi_client_t* client = llmapi_client_create(api_key, LLMAPI_URL_POE);
    if (!client) {
        fprintf(stderr, "Error: Failed to create client\n");
        return 1;
    }

    llmapi_client_set_model(client, "gpt-5");
    llmapi_client_add_system_message(client, "You are a helpful assistant.");

    printf("=== llmapi Basic Usage Demo (C API) ===\n\n");

    // Example 1: Non-streaming request
    printf("[Example 1] Non-streaming mode:\n");
    printf("Question: What is the capital of China?\n\n");
    
    llmapi_client_add_user_message(client, "What is the capital of China?");
    llmapi_client_request(client);

    llmapi_string_t answer = llmapi_client_get_answer(client);
    if (answer) {
        printf("Answer: %s\n\n", answer);
        llmapi_free_string(answer);
    } else {
        fprintf(stderr, "Error: %s\n\n", llmapi_get_last_error(client));
    }

    // Example 2: Streaming request
    printf("[Example 2] Streaming mode:\n");
    printf("Question: Convince me to use modern C++ (100 words)\n\n");
    
    llmapi_client_add_user_message(client, "Convince me to use modern C++ (100 words)");
    printf("Answer: ");
    llmapi_client_request_stream(client, stream_print, NULL);
    printf("\n\n");

    // Verify auto-save: get the last answer
    llmapi_string_t last_answer = llmapi_client_get_answer(client);
    printf("[Verification] Last answer length: %zu chars\n\n", last_answer ? strlen(last_answer) : 0);
    if (last_answer) llmapi_free_string(last_answer);

    // Example 3: Translate the story to Chinese
    printf("[Example 3] Translation (streaming):\n");
    printf("Question: 请把上个回答翻译成中文。\n\n");
    
    llmapi_client_add_user_message(client, "请把上个回答翻译成中文。");
    printf("Answer: ");
    llmapi_client_request_stream(client, stream_print, NULL);
    printf("\n\n=== Demo Complete ===\n");

    llmapi_client_destroy(client);
    return 0;
}
