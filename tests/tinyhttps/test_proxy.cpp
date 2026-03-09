import mcpplibs.tinyhttps;
import std;

#include <cassert>

int main() {
    using namespace mcpplibs::tinyhttps;
    Socket::platform_init();

    // Test 1: proxy URL parsing
    auto config = parse_proxy_url("http://proxy.example.com:8080");
    assert(config.host == "proxy.example.com");
    assert(config.port == 8080);

    // Test 2: proxy URL parsing without port (default 8080)
    auto config2 = parse_proxy_url("http://myproxy.com");
    assert(config2.host == "myproxy.com");
    assert(config2.port == 8080);

    // Test 3: HttpClient stores proxy config
    auto client = HttpClient(HttpClientConfig {
        .proxy = "http://127.0.0.1:8080",
    });
    assert(client.config().proxy.has_value());
    assert(client.config().proxy.value() == "http://127.0.0.1:8080");

    // Note: actual proxy connection test requires a real proxy,
    // so we only test config/parsing here

    Socket::platform_cleanup();
    std::println("test_proxy: ALL PASSED");
    return 0;
}
