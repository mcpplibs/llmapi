import mcpplibs.tinyhttps;
import std;

#include <cassert>
#include "../test_print.hpp"

int main() {
    using namespace mcpplibs::tinyhttps;

    // Test 1: platform init/cleanup
    Socket::platform_init();

    // Test 2: construct and validate
    Socket s;
    assert(!s.is_valid());

    // Test 3: connect to known host (httpbin.org:80)
    bool connected = s.connect("httpbin.org", 80, 5000);
    assert(connected);
    assert(s.is_valid());

    // Test 4: write HTTP request
    std::string req = "GET /get HTTP/1.1\r\nHost: httpbin.org\r\nConnection: close\r\n\r\n";
    int written = s.write(req.data(), static_cast<int>(req.size()));
    assert(written > 0);

    // Test 5: read response
    char buf[4096];
    int n = s.read(buf, sizeof(buf));
    assert(n > 0);
    std::string_view resp(buf, n);
    assert(resp.starts_with("HTTP/1.1 200"));

    // Test 6: close and validate
    s.close();
    assert(!s.is_valid());

    // Test 7: move semantics
    Socket s1;
    s1.connect("httpbin.org", 80, 5000);
    Socket s2 = std::move(s1);
    assert(!s1.is_valid());
    assert(s2.is_valid());
    s2.close();

    Socket::platform_cleanup();

    println("test_socket: ALL PASSED");
    return 0;
}
