export module mcpplibs.llmapi:coro;

import std;

export namespace mcpplibs::llmapi {

template<typename T>
class Task {
public:
    struct promise_type {
        std::optional<T> value;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T val) { value = std::move(val); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

private:
    std::coroutine_handle<promise_type> handle_;

public:
    explicit Task(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }

    // Move only
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // Awaitable
    bool await_ready() const noexcept { return handle_.done(); }
    void await_suspend(std::coroutine_handle<> awaiter) noexcept {
        handle_.resume();
        awaiter.resume();
    }
    T await_resume() {
        if (handle_.promise().exception)
            std::rethrow_exception(handle_.promise().exception);
        return std::move(*handle_.promise().value);
    }

    // Sync get
    T get() {
        if (!handle_.done()) handle_.resume();
        if (handle_.promise().exception)
            std::rethrow_exception(handle_.promise().exception);
        return std::move(*handle_.promise().value);
    }
};

// Task<void> specialization
template<>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception;
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

private:
    std::coroutine_handle<promise_type> handle_;

public:
    explicit Task(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    bool await_ready() const noexcept { return handle_.done(); }
    void await_suspend(std::coroutine_handle<> awaiter) noexcept {
        handle_.resume();
        awaiter.resume();
    }
    void await_resume() {
        if (handle_.promise().exception)
            std::rethrow_exception(handle_.promise().exception);
    }
    void get() {
        if (!handle_.done()) handle_.resume();
        if (handle_.promise().exception)
            std::rethrow_exception(handle_.promise().exception);
    }
};

} // namespace mcpplibs::llmapi
