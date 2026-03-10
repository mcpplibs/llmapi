// Minimal example - simplest way to use llmapi
import mcpplibs.llmapi;
import std;

#include "print.hpp"

using namespace mcpplibs::llmapi;

int main() {
    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        println("Error: OPENAI_API_KEY not set");
        return 1;
    }

    auto client = Client(openai::OpenAI({
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    }));

    auto resp = client.chat("Hello! In one sentence, introduce modern C++.");
    println(resp.text());

    return 0;
}
