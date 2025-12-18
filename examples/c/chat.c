// Simple and elegant AI chat CLI tool (C API) using streaming
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

    client->set_model(client, "gpt-5");
    client->add_system_message(client, "You are a helpful assistant.");

    printf("AI Chat CLI (C API) - Type 'quit' to exit\n\n");

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

        client->add_user_message(client, input);
        printf("\nAI: ");
        client->request_stream(client, stream_print, NULL);
        printf("\n\n");
    }

    client->destroy(client);
    return 0;
}
