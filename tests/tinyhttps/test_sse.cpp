import mcpplibs.tinyhttps;
import std;

#include <cassert>

int main() {
    using namespace mcpplibs::tinyhttps;

    SseParser parser;

    // Test 1: simple data event
    auto events = parser.feed("data: hello\n\n");
    assert(events.size() == 1);
    assert(events[0].data == "hello");
    assert(events[0].event == "message");

    // Test 2: named event
    events = parser.feed("event: ping\ndata: {}\n\n");
    assert(events.size() == 1);
    assert(events[0].event == "ping");
    assert(events[0].data == "{}");

    // Test 3: multi-line data
    events = parser.feed("data: line1\ndata: line2\n\n");
    assert(events.size() == 1);
    assert(events[0].data == "line1\nline2");

    // Test 4: chunked feed (data arrives in pieces)
    parser.reset();
    events = parser.feed("data: hel");
    assert(events.empty());
    events = parser.feed("lo\n\n");
    assert(events.size() == 1);
    assert(events[0].data == "hello");

    // Test 5: multiple events in one chunk
    events = parser.feed("data: first\n\ndata: second\n\n");
    assert(events.size() == 2);
    assert(events[0].data == "first");
    assert(events[1].data == "second");

    // Test 6: OpenAI format
    parser.reset();
    events = parser.feed("data: {\"choices\":[{\"delta\":{\"content\":\"Hi\"}}]}\n\n");
    assert(events.size() == 1);
    assert(events[0].data.find("Hi") != std::string::npos);

    // Test 7: Anthropic format (event type line)
    parser.reset();
    events = parser.feed("event: content_block_delta\ndata: {\"type\":\"content_block_delta\"}\n\n");
    assert(events.size() == 1);
    assert(events[0].event == "content_block_delta");

    // Test 8: [DONE] sentinel
    parser.reset();
    events = parser.feed("data: [DONE]\n\n");
    assert(events.size() == 1);
    assert(events[0].data == "[DONE]");

    // Test 9: comment lines (start with :) ignored
    parser.reset();
    events = parser.feed(": this is a comment\ndata: actual\n\n");
    assert(events.size() == 1);
    assert(events[0].data == "actual");

    // Test 10: id field
    parser.reset();
    events = parser.feed("id: 123\ndata: msg\n\n");
    assert(events.size() == 1);
    assert(events[0].id == "123");
    assert(events[0].data == "msg");

    std::println("test_sse: ALL PASSED");
    return 0;
}
