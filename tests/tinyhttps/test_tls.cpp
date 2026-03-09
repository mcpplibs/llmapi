import mcpplibs.tinyhttps;
import std;

#include <cassert>

int main() {
    using namespace mcpplibs::tinyhttps;
    Socket::platform_init();

    // Test 1: TLS connect to HTTPS host
    TlsSocket tls;
    bool connected = tls.connect("httpbin.org", 443, 5000, true);
    assert(connected);
    assert(tls.is_valid());

    // Test 2: HTTPS request over TLS
    std::string req = "GET /get HTTP/1.1\r\nHost: httpbin.org\r\nConnection: close\r\n\r\n";
    int written = tls.write(req.data(), static_cast<int>(req.size()));
    assert(written > 0);

    // Test 3: Read HTTPS response
    std::string response;
    char buf[4096];
    int n;
    while ((n = tls.read(buf, sizeof(buf))) > 0) {
        response.append(buf, n);
    }
    assert(response.starts_with("HTTP/1.1 200"));
    assert(response.find("\"url\"") != std::string::npos);

    // Test 4: close
    tls.close();
    assert(!tls.is_valid());

    // Test 5: move semantics
    TlsSocket t1;
    t1.connect("httpbin.org", 443, 5000, true);
    TlsSocket t2 = std::move(t1);
    assert(!t1.is_valid());
    assert(t2.is_valid());
    t2.close();

    Socket::platform_cleanup();
    std::println("test_tls: ALL PASSED");
    return 0;
}
