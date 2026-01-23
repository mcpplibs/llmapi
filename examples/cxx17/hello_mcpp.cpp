// Minimal example - simplest way to use llmapi
#include "llmapi.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    using namespace mcpplibs;

    llmapi::Client client(std::getenv("OPENAI_API_KEY"), llmapi::URL::Poe);

    client.model("gpt-5")
          .system("You are a helpful assistant.")
          .user("In one sentence, introduce modern C++. 并给出中文翻译")
          .request([](std::string_view chunk) {
                std::cout << chunk;
                std::cout.flush();
          });

    return 0;
}