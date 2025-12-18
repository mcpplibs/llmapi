module;

#include <curl/curl.h>

export module mcpplibs.llmapi.openai;

import std;
import nlohmann.json;

import mcpplibs.llmapi.url;

export namespace mcpplibs::llmapi::openai {

using Json = nlohmann::json;

// Concept to constrain callback type
template<typename F>
concept StreamCallback = std::invocable<F, std::string_view> && 
                        std::same_as<std::invoke_result_t<F, std::string_view>, void>;

class OpenAI {
    std::string mApiKey;
    std::string mBaseUrl;
    std::string mModel;
    std::string mEndpoint;
    Json mMessages;

public:
    OpenAI(std::string_view apiKey, std::string_view baseUrl = llmapi::URL::OpenAI)
        : mApiKey(apiKey), 
        mBaseUrl(baseUrl),
        mMessages(Json::array())
    {
        if (mApiKey.empty()) {
            throw std::runtime_error("API key cannot be empty");
        }
    }

    // add safe check for const char* overload - example: std::getenv("KEY")
    OpenAI(const char* apiKey, std::string_view baseUrl = llmapi::URL::OpenAI)
        : OpenAI(std::string_view(apiKey ? apiKey : ""), baseUrl) { }

    // Rule of five - explicitly defaulted
    OpenAI(const OpenAI&) = default;
    OpenAI(OpenAI&&) = default;
    OpenAI& operator=(const OpenAI&) = default;
    OpenAI& operator=(OpenAI&&) = default;
    ~OpenAI() = default;

public: // config methods (chainable)

    OpenAI& model(std::string_view model) {
        mEndpoint = mBaseUrl + "/chat/completions";
        mModel = model;
        return *this;
    }

public: // Message methods

    // Add messages
    OpenAI& add_message(std::string_view role, std::string_view content) {
        mMessages.push_back({
            {"role", role},
            {"content", content}
        });
        return *this;
    }

    OpenAI& user(std::string_view content) {
        return add_message("user", content);
    }

    OpenAI& system(std::string_view content) {
        return add_message("system", content);
    }

    OpenAI& assistant(std::string_view content) {
        return add_message("assistant", content);
    }

    // Clear conversation history
    OpenAI& clear() {
        mMessages = Json::array();
        return *this;
    }

public:

    // Getters
    std::string_view getApiKey() const { return mApiKey; }
    std::string_view getBaseUrl() const { return mBaseUrl; }
    std::string_view getModel() const { return mModel; }

    Json getMessages() const { return mMessages; }
    int getMessageCount() const { return static_cast<int>(mMessages.size()) / 2; }

    std::string getAnswer() const {
        if (mMessages.empty()) return "";
        const auto& lastMessage = mMessages.back();
        if (lastMessage.contains("role") && lastMessage["role"] == "assistant" &&
            lastMessage.contains("content")) {
            return lastMessage["content"].get<std::string>();
        }
        return "";
    }

public: // Request methods

    // Execute request (non-streaming) - auto saves assistant reply
    Json request() {
        validate_request();
        auto response = send_request(build_payload(mMessages, false));
        
        // Auto-save assistant reply to conversation history
        if (response.contains("choices") && !response["choices"].empty()) {
            auto& choice = response["choices"][0];
            if (choice.contains("message") && choice["message"].contains("content")) {
                std::string content = choice["message"]["content"];
                assistant(content);
            }
        }
        
        return response;
    }

    // One-shot request without building conversation (non-streaming)
    Json request(const Json& messages) {
        validate_request();
        return send_request(build_payload(messages, false));
    }

    // Execute request with callback (streaming) - auto saves assistant reply
    template<StreamCallback Callback>
    void request(Callback&& callback) {
        validate_request();
        
        // Wrapper to collect full response
        std::string full_response;
        auto wrapper_callback = [&full_response, &callback](std::string_view chunk) {
            full_response += chunk;
            callback(chunk);
        };
        
        send_stream_request(build_payload(mMessages, true), wrapper_callback);
        
        // Auto-save assistant reply to conversation history
        if (!full_response.empty()) {
            assistant(full_response);
        }
    }

private:
    struct StreamContext {
        std::function<void(std::string_view)> callback;
        std::string buffer;
    };

    // Validate request preconditions
    void validate_request() const {
        if (mEndpoint.empty()) {
            throw std::runtime_error("Endpoint not set. Call model() first.");
        }
        if (mModel.empty()) {
            throw std::runtime_error("Model not set.");
        }
    }

    // Build request payload
    Json build_payload(const Json& messages, bool stream) const {
        Json payload;
        payload["model"] = mModel;
        payload["messages"] = messages;
        if (stream) {
            payload["stream"] = true;
        }
        return payload;
    }

    // Setup common CURL headers
    struct curl_slist* setup_headers() const {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string authHeader = "Authorization: Bearer " + mApiKey;
        headers = curl_slist_append(headers, authHeader.c_str());
        return headers;
    }

    Json send_request(const Json& payload) {
        std::string payloadStr = payload.dump();
        std::string response;

        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        // Set up headers
        struct curl_slist* headers = setup_headers();

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, mEndpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        
        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL error: ") + curl_easy_strerror(res));
        }

        return Json::parse(response);
    }

    template<StreamCallback Callback>
    void send_stream_request(const Json& payload, Callback&& callback) {
        std::string payloadStr = payload.dump();

        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        StreamContext context;
        context.callback = std::forward<Callback>(callback);

        // Set up headers
        struct curl_slist* headers = setup_headers();

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, mEndpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, streamCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &context);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        
        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL error: ") + curl_easy_strerror(res));
        }
    }

    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t totalSize = size * nmemb;
        std::string* response = static_cast<std::string*>(userp);
        response->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    static size_t streamCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t totalSize = size * nmemb;
        StreamContext* context = static_cast<StreamContext*>(userp);
        
        std::string_view data(static_cast<char*>(contents), totalSize);
        context->buffer.append(data);

        // Process SSE data line by line
        size_t pos = 0;
        while ((pos = context->buffer.find('\n')) != std::string::npos) {
            std::string line = context->buffer.substr(0, pos);
            context->buffer.erase(0, pos + 1);

            // Remove \r if present
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            // Check for data: prefix
            if (line.starts_with("data: ")) {
                std::string jsonStr = line.substr(6);
                
                // Check for [DONE] message
                if (jsonStr == "[DONE]") {
                    continue;
                }

                try {
                    auto chunk = Json::parse(jsonStr);
                    
                    // Extract content from the chunk
                    if (chunk.contains("choices") && !chunk["choices"].empty()) {
                        auto& choice = chunk["choices"][0];
                        
                        // For chat completions streaming
                        if (choice.contains("delta") && choice["delta"].contains("content")) {
                            std::string content = choice["delta"]["content"];
                            context->callback(content);
                        }
                        // For responses endpoint streaming
                        else if (choice.contains("message") && choice["message"].contains("content")) {
                            std::string content = choice["message"]["content"];
                            context->callback(content);
                        }
                    }
                } catch (const Json::exception& e) {
                    // Silently ignore JSON parsing errors in streaming
                }
            }
        }

        return totalSize;
    }
};

} // namespace mcpplibs::openai