# llmapi

> 使用 C++23 模块构建的现代 LLM 客户端

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| [English](README.md) - 简体中文 - [繁體中文](README.zh.hant.md) |
|:---:|
| [文档导航](docs/README.md) - [中文文档](docs/zh/README.md) - [English Docs](docs/en/README.md) - [繁體中文文件](docs/zh-hant/README.md) |

`llmapi` 提供类型化的 `Client<Provider>` API，覆盖聊天、流式输出、嵌入、工具调用与对话持久化。默认别名 `Config` 对应 OpenAI 风格配置，常见场景不需要显式写 `openai::OpenAI(...)`。

## 特性

- C++23 模块：`import mcpplibs.llmapi`
- 强类型消息、工具与响应结构
- 同步、异步、流式聊天接口
- OpenAI Provider 支持 embeddings
- 支持保存 / 加载对话历史
- 可通过 `baseUrl` 访问 OpenAI 兼容端点

## 快速开始

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::cerr << "OPENAI_API_KEY not set\n";
        return 1;
    }

    auto client = Client(Config{
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    });

    client.system("You are a concise assistant.");
    auto resp = client.chat("用两句话解释 C++23 模块的价值。");

    std::cout << resp.text() << '\n';
    return 0;
}
```

## Provider

- `Config`：`openai::Config` 的导出别名，默认走 OpenAI 风格
- `openai::OpenAI`：OpenAI 聊天、流式、工具调用、embeddings
- `AnthropicConfig` / `anthropic::Anthropic`：Anthropic 聊天与流式

兼容端点示例：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## 构建与运行

```bash
xmake
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## 包管理使用

```lua
add_repositories("mcpplibs-index https://github.com/mcpplibs/mcpplibs-index.git")
add_requires("llmapi 0.1.0")

target("demo")
    set_kind("binary")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_files("src/*.cpp")
    add_packages("llmapi")
```

更多内容见 [docs/zh/getting-started.md](docs/zh/getting-started.md)、[docs/zh/providers.md](docs/zh/providers.md) 与 [docs/zh/README.md](docs/zh/README.md)。

## 许可证

Apache-2.0，详见 [LICENSE](LICENSE)
