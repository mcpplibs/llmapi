export module mcpplibs.llmapi:errors;

import std;

export namespace mcpplibs::llmapi {

// Base API error with HTTP status and structured error info
class ApiError : public std::runtime_error {
public:
    int statusCode;
    std::string type;
    std::string body;

    ApiError(int status, std::string errorType, std::string errorBody, const std::string& message)
        : std::runtime_error(message)
        , statusCode(status)
        , type(std::move(errorType))
        , body(std::move(errorBody))
    {}
};

// Network/connection errors (DNS, TLS, timeout)
class ConnectionError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace mcpplibs::llmapi
