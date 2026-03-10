export module mcpplibs.llmapi:client;

import :types;
import :provider;
import :coro;
import :openai;
import :anthropic;
import std;

export namespace mcpplibs::llmapi {

template<Provider P>
class Client {
private:
    P provider_;
    Conversation conversation_;
    ChatParams defaultParams_;

public:
    explicit Client(P provider) : provider_(std::move(provider)) {}
    explicit Client(openai::Config config)
        requires std::same_as<P, openai::OpenAI>
        : provider_(openai::OpenAI(std::move(config))) {}
    explicit Client(anthropic::Config config)
        requires std::same_as<P, anthropic::Anthropic>
        : provider_(anthropic::Anthropic(std::move(config))) {}

    // Config (chainable)
    Client& default_params(ChatParams params) {
        defaultParams_ = std::move(params);
        return *this;
    }

    // Message management
    Client& system(std::string_view content) {
        conversation_.push(Message::system(content));
        return *this;
    }
    Client& user(std::string_view content) {
        conversation_.push(Message::user(content));
        return *this;
    }
    Client& add_message(Message msg) {
        conversation_.push(std::move(msg));
        return *this;
    }
    Client& clear() {
        conversation_.clear();
        return *this;
    }

    // Sync chat
    ChatResponse chat(std::string_view userMessage) {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat(conversation_.messages, defaultParams_);
        conversation_.push(Message::assistant(response.text()));
        return response;
    }
    ChatResponse chat(std::string_view userMessage, ChatParams params) {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat(conversation_.messages, params);
        conversation_.push(Message::assistant(response.text()));
        return response;
    }

    // Async chat (synchronous implementation wrapped as Task to avoid GCC coroutine-in-template issues)
    Task<ChatResponse> chat_async(std::string_view userMessage) {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat(conversation_.messages, defaultParams_);
        conversation_.push(Message::assistant(response.text()));
        co_return response;
    }

    // Streaming (requires StreamableProvider)
    ChatResponse chat_stream(std::string_view userMessage,
                             std::function<void(std::string_view)> callback)
        requires StreamableProvider<P>
    {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat_stream(conversation_.messages, defaultParams_, std::move(callback));
        conversation_.push(Message::assistant(response.text()));
        return response;
    }

    Task<ChatResponse> chat_stream_async(std::string_view userMessage,
                                          std::function<void(std::string_view)> callback)
        requires StreamableProvider<P>
    {
        conversation_.push(Message::user(userMessage));
        auto response = provider_.chat_stream(conversation_.messages, defaultParams_, std::move(callback));
        conversation_.push(Message::assistant(response.text()));
        co_return response;
    }

    // Embeddings (requires EmbeddableProvider)
    EmbeddingResponse embed(const std::vector<std::string>& inputs, std::string_view model)
        requires EmbeddableProvider<P>
    {
        return provider_.embed(inputs, model);
    }

    // Conversation access
    const Conversation& conversation() const { return conversation_; }
    Conversation& conversation() { return conversation_; }

    void save_conversation(std::string_view filePath) const {
        conversation_.save(filePath);
    }
    void load_conversation(std::string_view filePath) {
        conversation_ = Conversation::load(filePath);
    }

    // Provider access
    const P& provider() const { return provider_; }
    P& provider() { return provider_; }
};

Client(openai::Config) -> Client<openai::OpenAI>;
Client(anthropic::Config) -> Client<anthropic::Anthropic>;

} // namespace mcpplibs::llmapi
