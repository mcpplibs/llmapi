import mcpplibs.tinyhttps;
import std;

#include <cassert>
#include "../test_print.hpp"

int main() {
    using namespace mcpplibs::tinyhttps;
    Socket::platform_init();

    HttpClient client(HttpClientConfig {
        .connectTimeoutMs = 10000,
        .readTimeoutMs = 30000,
        .verifySsl = true,
        .keepAlive = true,
    });

    // Test 1: full HTTPS POST (simulates LLM API call pattern)
    auto resp = client.send(HttpRequest {
        .method = Method::POST,
        .url = "https://httpbin.org/post",
        .headers = {
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer test-key"},
            {"X-Custom-Header", "custom-value"},
        },
        .body = R"({"model":"test","messages":[{"role":"user","content":"hi"}]})",
    });

    assert(resp.ok());
    assert(resp.statusCode == 200);
    assert(resp.body.find("test-key") != std::string::npos);
    assert(resp.body.find("custom-value") != std::string::npos);
    assert(resp.body.find("\"model\"") != std::string::npos);

    // Test 2: connection reuse (second request to same host)
    auto resp2 = client.send(HttpRequest {
        .method = Method::GET,
        .url = "https://httpbin.org/get",
    });
    assert(resp2.ok());

    // Test 3: SSE parser standalone verification
    SseParser parser;
    auto events = parser.feed("event: content_block_delta\ndata: {\"text\":\"hello\"}\n\n");
    assert(events.size() == 1);
    assert(events[0].event == "content_block_delta");

    Socket::platform_cleanup();
    println("test_integration (tinyhttps): ALL PASSED");
    return 0;
}
