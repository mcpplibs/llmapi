export module mcpplibs.llmapi:provider;

import std;

export import :types;
export import :coro;

export namespace mcpplibs::llmapi {

template<typename F>
concept StreamCallback = std::invocable<F, std::string_view> &&
                         std::same_as<std::invoke_result_t<F, std::string_view>, void>;

template<typename P>
concept Provider = requires(P p, const std::vector<Message>& messages, const ChatParams& params) {
    { p.name() } -> std::convertible_to<std::string_view>;
    { p.chat(messages, params) } -> std::same_as<ChatResponse>;
    { p.chat_async(messages, params) } -> std::same_as<Task<ChatResponse>>;
};

template<typename P>
concept StreamableProvider = Provider<P> && requires(P p,
    const std::vector<Message>& messages, const ChatParams& params,
    std::function<void(std::string_view)> cb) {
    { p.chat_stream(messages, params, cb) } -> std::same_as<ChatResponse>;
    { p.chat_stream_async(messages, params, cb) } -> std::same_as<Task<ChatResponse>>;
};

template<typename P>
concept EmbeddableProvider = Provider<P> && requires(P p,
    const std::vector<std::string>& inputs, std::string_view model) {
    { p.embed(inputs, model) } -> std::same_as<EmbeddingResponse>;
};

} // namespace mcpplibs::llmapi
