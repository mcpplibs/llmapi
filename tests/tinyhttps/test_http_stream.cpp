import mcpplibs.tinyhttps;
import std;

#include <cassert>
#include "../test_print.hpp"

int main() {
    using namespace mcpplibs::tinyhttps;
    Socket::platform_init();

    HttpClient client;

    // Test 1: Stream from a server that returns chunked data
    // httpbin.org/stream/3 returns 3 JSON lines (newline-delimited JSON, not SSE)
    // The SseParser won't produce events from plain JSON lines (no "data:" prefix),
    // but the streaming read path is exercised and should complete without error.
    int lineCount { 0 };
    auto resp = client.send_stream(
        HttpRequest {
            .method = Method::GET,
            .url = "https://httpbin.org/stream/3",
        },
        [&lineCount](const SseEvent& event) -> bool {
            lineCount++;
            println("Event ", lineCount, ": ", event.data.substr(0, 50));
            return true;
        }
    );
    assert(resp.statusCode == 200);
    println("Stream/3 completed, received ", lineCount, " events, status ", resp.statusCode);

    // Test 2: early stop (return false from callback)
    int stopCount { 0 };
    auto resp2 = client.send_stream(
        HttpRequest {
            .method = Method::GET,
            .url = "https://httpbin.org/stream/10",
        },
        [&stopCount](const SseEvent&) -> bool {
            stopCount++;
            return stopCount < 2; // stop after processing 1 event
        }
    );
    assert(resp2.statusCode == 200);
    println("Stopped after ", stopCount, " events");

    // Test 3: verify response headers are captured
    auto resp3 = client.send_stream(
        HttpRequest {
            .method = Method::GET,
            .url = "https://httpbin.org/stream/1",
        },
        [](const SseEvent&) -> bool { return true; }
    );
    assert(resp3.statusCode == 200);
    assert(!resp3.headers.empty());
    println("Response has ", resp3.headers.size(), " headers");

    Socket::platform_cleanup();
    println("test_http_stream: ALL PASSED");
    return 0;
}
