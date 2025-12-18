// C API implementation that wraps the C++ module implementation
#include "../include/llmapi.h"

import std;
import mcpplibs.llmapi;

using namespace mcpplibs::llmapi;

// Private implementation data
struct llmapi_client_private_t {
    Client* cpp_client;
    std::string last_error;
    
    llmapi_client_private_t(Client* client) : cpp_client(client) {}
    
    ~llmapi_client_private_t() {
        delete cpp_client;
    }
};

// Helper to get private data
static inline llmapi_client_private_t* get_private(llmapi_client_t* self) {
    return static_cast<llmapi_client_private_t*>(self->_private);
}

// Helper to handle exceptions and set error messages
template<typename Func>
static llmapi_error_t safe_call(llmapi_client_t* self, Func&& func) {
    if (!self || !self->_private) {
        return LLMAPI_ERROR_INVALID_PARAM;
    }
    
    auto* priv = get_private(self);
    
    try {
        func();
        priv->last_error.clear();
        return LLMAPI_OK;
    } catch (const std::invalid_argument& e) {
        priv->last_error = e.what();
        return LLMAPI_ERROR_INVALID_PARAM;
    } catch (const std::runtime_error& e) {
        priv->last_error = e.what();
        return LLMAPI_ERROR_STATE;
    } catch (const Json::exception& e) {
        priv->last_error = e.what();
        return LLMAPI_ERROR_PARSE;
    } catch (const std::exception& e) {
        priv->last_error = e.what();
        return LLMAPI_ERROR_UNKNOWN;
    } catch (...) {
        priv->last_error = "Unknown error";
        return LLMAPI_ERROR_UNKNOWN;
    }
}

// Member function implementations
static void llmapi_client_destroy_impl(llmapi_client_t* self) {
    if (self) {
        if (self->_private) {
            delete get_private(self);
        }
        delete self;
    }
}

static llmapi_error_t llmapi_client_set_model_impl(llmapi_client_t* self, const char* model) {
    if (!model) {
        return LLMAPI_ERROR_INVALID_PARAM;
    }
    
    return safe_call(self, [&]() {
        get_private(self)->cpp_client->model(model);
    });
}

static llmapi_error_t llmapi_client_add_user_message_impl(llmapi_client_t* self, const char* content) {
    if (!content) {
        return LLMAPI_ERROR_INVALID_PARAM;
    }
    
    return safe_call(self, [&]() {
        get_private(self)->cpp_client->user(content);
    });
}

static llmapi_error_t llmapi_client_add_system_message_impl(llmapi_client_t* self, const char* content) {
    if (!content) {
        return LLMAPI_ERROR_INVALID_PARAM;
    }
    
    return safe_call(self, [&]() {
        get_private(self)->cpp_client->system(content);
    });
}

static llmapi_error_t llmapi_client_add_assistant_message_impl(llmapi_client_t* self, const char* content) {
    if (!content) {
        return LLMAPI_ERROR_INVALID_PARAM;
    }
    
    return safe_call(self, [&]() {
        get_private(self)->cpp_client->assistant(content);
    });
}

static llmapi_error_t llmapi_client_clear_impl(llmapi_client_t* self) {
    return safe_call(self, [&]() {
        get_private(self)->cpp_client->clear();
    });
}

static int llmapi_client_get_message_count_impl(llmapi_client_t* self) {
    if (!self || !self->_private) {
        return -1;
    }
    
    try {
        return get_private(self)->cpp_client->getMessageCount();
    } catch (...) {
        return -1;
    }
}

static llmapi_string_t llmapi_client_get_answer_impl(llmapi_client_t* self) {
    if (!self || !self->_private) {
        return nullptr;
    }
    
    try {
        std::string content = get_private(self)->cpp_client->getAnswer();
        
        if (content.empty()) {
            return nullptr;
        }
        
        // Allocate C string
        char* result = static_cast<char*>(std::malloc(content.size() + 1));
        if (result) {
            std::memcpy(result, content.c_str(), content.size());
            result[content.size()] = '\0';
        }
        
        return result;
    } catch (...) {
        return nullptr;
    }
}

static llmapi_string_t llmapi_client_request_impl(llmapi_client_t* self) {
    if (!self || !self->_private) {
        return nullptr;
    }
    
    auto* priv = get_private(self);
    
    try {
        auto response = priv->cpp_client->request();
        
        // Extract content from JSON response
        std::string content;
        if (response.contains("choices") && !response["choices"].empty()) {
            auto& choice = response["choices"][0];
            if (choice.contains("message") && choice["message"].contains("content")) {
                content = choice["message"]["content"].get<std::string>();
            }
        }
        
        if (content.empty()) {
            priv->last_error = "No content in response";
            return nullptr;
        }
        
        // Allocate C string
        char* result = static_cast<char*>(std::malloc(content.size() + 1));
        if (result) {
            std::memcpy(result, content.c_str(), content.size());
            result[content.size()] = '\0';
        }
        
        priv->last_error.clear();
        return result;
    } catch (const std::exception& e) {
        priv->last_error = e.what();
        return nullptr;
    } catch (...) {
        priv->last_error = "Unknown error";
        return nullptr;
    }
}

static llmapi_error_t llmapi_client_request_stream_impl(
    llmapi_client_t* self,
    llmapi_stream_callback_t callback,
    void* user_data
) {
    if (!callback) {
        return LLMAPI_ERROR_INVALID_PARAM;
    }
    
    return safe_call(self, [&]() {
        get_private(self)->cpp_client->request([callback, user_data](std::string_view content) {
            callback(content.data(), content.size(), user_data);
        });
    });
}

static const char* llmapi_get_last_error_impl(llmapi_client_t* self) {
    if (!self || !self->_private) {
        return "Invalid client";
    }
    
    auto* priv = get_private(self);
    return priv->last_error.empty() ? nullptr : priv->last_error.c_str();
}

extern "C" {

llmapi_client_t* llmapi_client_create(const char* api_key, const char* base_url) {
    if (!api_key) {
        return nullptr;
    }
    
    try {
        // Create the client object
        llmapi_client_t* client = new llmapi_client_t();
        
        // Initialize function pointers (virtual function table)
        client->destroy = llmapi_client_destroy_impl;
        client->set_model = llmapi_client_set_model_impl;
        client->add_user_message = llmapi_client_add_user_message_impl;
        client->add_system_message = llmapi_client_add_system_message_impl;
        client->add_assistant_message = llmapi_client_add_assistant_message_impl;
        client->clear = llmapi_client_clear_impl;
        client->get_message_count = llmapi_client_get_message_count_impl;
        client->get_answer = llmapi_client_get_answer_impl;
        client->request = llmapi_client_request_impl;
        client->request_stream = llmapi_client_request_stream_impl;
        client->get_last_error = llmapi_get_last_error_impl;

        // Create private implementation data
        std::string_view base_url_sv = base_url ? base_url : LLMAPI_URL_OPENAI;
        Client* cpp_client = new Client(api_key, base_url_sv);
        client->_private = new llmapi_client_private_t(cpp_client);
        
        return client;
    } catch (...) {
        return nullptr;
    }
}

void llmapi_free_string(llmapi_string_t str) {
    std::free(str);
}

} // extern "C"
