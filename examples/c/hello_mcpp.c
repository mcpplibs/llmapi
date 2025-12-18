// Minimal example (C API) - simplest way to use llmapi
#include <stdio.h>
#include <stdlib.h>
#include "llmapi.h"

void stream_print(const char* s, size_t len, void* data) {
    printf("%.*s", (int)len, s);
    fflush(stdout);
}

int main(void) {
    llmapi_client_t* c = llmapi_client_create(getenv("OPENAI_API_KEY"), LLMAPI_URL_POE);

    c->set_model(c, "gpt-5");
    c->add_system_message(c, "You are a helpful assistant.");
    c->add_user_message(c, "In one sentence, introduce modern C++. 并给出中文翻译");
    c->request_stream(c, stream_print, NULL);
    
    c->destroy(c);
    return 0;
}