import mcpplibs.llmapi;
import std;

#include <cassert>
#include "../test_print.hpp"

int main() {
    using namespace mcpplibs::llmapi;

    Conversation conv;
    conv.push(Message::system("you are helpful"));
    conv.push(Message::user("hello"));
    conv.push(Message::assistant("hi there"));
    conv.push(Message {
        .role = Role::User,
        .content = std::vector<ContentPart>{
            TextContent { "look at this" },
            ImageContent { .data = "base64data", .mediaType = "image/png" },
        },
    });

    // Save
    std::string path = "/tmp/test_conv.json";
    conv.save(path);

    // Load
    auto loaded = Conversation::load(path);
    assert(loaded.size() == conv.size());

    // Verify content preserved
    assert(std::get<std::string>(loaded.messages[0].content) == "you are helpful");
    assert(loaded.messages[0].role == Role::System);
    assert(std::get<std::string>(loaded.messages[1].content) == "hello");

    // Verify multimodal preserved
    auto& parts = std::get<std::vector<ContentPart>>(loaded.messages[3].content);
    assert(parts.size() == 2);
    auto& img = std::get<ImageContent>(parts[1]);
    assert(img.mediaType == "image/png");

    // Cleanup
    std::filesystem::remove(path);

    println("test_serialization: ALL PASSED");
    return 0;
}
