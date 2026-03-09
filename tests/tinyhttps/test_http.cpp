import mcpplibs.tinyhttps;
import std;

#include <cassert>

int main() {
    using namespace mcpplibs::tinyhttps;
    Socket::platform_init();

    HttpClient client;

    // Test 1: simple GET
    auto resp = client.send(HttpRequest {
        .method = Method::GET,
        .url = "https://httpbin.org/get",
        .headers = {{"Accept", "application/json"}},
    });
    assert(resp.ok());
    assert(resp.statusCode == 200);
    assert(resp.body.find("\"url\"") != std::string::npos);

    // Test 2: POST with JSON body
    auto resp2 = client.send(HttpRequest::post(
        "https://httpbin.org/post",
        R"({"key":"value"})"
    ));
    assert(resp2.ok());
    assert(resp2.body.find("\"key\"") != std::string::npos);

    // Test 3: custom headers
    auto resp3 = client.send(HttpRequest {
        .method = Method::GET,
        .url = "https://httpbin.org/headers",
        .headers = {{"X-Custom", "test123"}},
    });
    assert(resp3.ok());
    assert(resp3.body.find("test123") != std::string::npos);

    // Test 4: 404 handling
    auto resp4 = client.send(HttpRequest {
        .method = Method::GET,
        .url = "https://httpbin.org/status/404",
    });
    assert(!resp4.ok());
    assert(resp4.statusCode == 404);

    Socket::platform_cleanup();
    std::println("test_http: ALL PASSED");
    return 0;
}
