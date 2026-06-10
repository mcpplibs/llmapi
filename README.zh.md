# llmapi

> 使用 C++23 模块构建的现代 LLM 客户端 — `import` 即聊天 · OpenAI 兼容 · 模板开箱即用

[![Release](https://img.shields.io/github/v/release/mcpplibs/llmapi)](https://github.com/mcpplibs/llmapi/releases)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| [English](README.md) - 简体中文 - [繁體中文](README.zh.hant.md) |
|:---:|
| [mcpp 构建工具](https://github.com/mcpp-community/mcpp) · [包索引 mcpp-index](https://github.com/mcpp-community/mcpp-index) · [文档导航](docs/README.md) · [Issues](https://github.com/mcpplibs/llmapi/issues) |

`llmapi` 提供类型化的 `Client<Provider>` API，覆盖聊天、流式输出、嵌入、工具调用与对话持久化。默认别名 `Config` 对应 OpenAI 风格配置，常见场景不需要显式写 `openai::OpenAI(...)`。

## 快速开始(mcpp)

```bash
mcpp new myagent --template llmapi && cd myagent
export OPENAI_API_KEY="sk-..."
mcpp run
```

模板随库分发、版本自动对齐:

```bash
mcpp new --list-templates llmapi              # 列出库提供的模板
mcpp new mybot --template llmapi:chat        # 交互式流式聊天 CLI
mcpp new mybot --template llmapi:anthropic   # Anthropic Provider
mcpp new mybot --template llmapi:deepseek    # OpenAI 兼容端点(DeepSeek)
```

或在已有 mcpp 项目中接入:

```bash
mcpp add llmapi
```

```toml
[dependencies.mcpplibs]
llmapi = "0.2.7"
```

```cpp
import mcpplibs.llmapi;
import std;

int main() {
    using namespace mcpplibs::llmapi;

    auto apiKey = std::getenv("OPENAI_API_KEY");
    if (!apiKey) {
        std::println(stderr, "OPENAI_API_KEY not set");
        return 1;
    }

    auto client = Client(Config{
        .apiKey = apiKey,
        .model = "gpt-4o-mini",
    });

    client.system("You are a concise assistant.");
    auto resp = client.chat("用两句话解释 C++23 模块的价值。");

    std::println("{}", resp.text());
    return 0;
}
```

## 特性

- C++23 模块：`import mcpplibs.llmapi`
- 强类型消息、工具与响应结构
- 同步、异步、流式聊天接口
- OpenAI Provider 支持 embeddings
- 支持保存 / 加载对话历史
- 可通过 `baseUrl` 访问 OpenAI 兼容端点
- 项目模板：`mcpp new <name> --template llmapi[:<template>]`

## 模板

| 模板 | 说明 |
|---|---|
| `openai`(默认) | 最小 OpenAI 聊天 — 一问一答 |
| `chat` | 交互式流式聊天 CLI(OpenAI) |
| `anthropic` | Anthropic(Claude)聊天 |
| `deepseek` | 通过 `baseUrl` 走 OpenAI 兼容端点(DeepSeek) |

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

## 源码构建

```bash
git clone https://github.com/mcpplibs/llmapi.git && cd llmapi
mcpp build
```

## 文档

- [快速上手](docs/zh/getting-started.md)
- [Provider 配置](docs/zh/providers.md)
- [C++ API 指南](docs/zh/cpp-api.md)
- [使用 xmake 集成 llmapi](docs/zh/xmake.md)
- [文档导航](docs/README.md)

## 许可证

Apache-2.0，详见 [LICENSE](LICENSE)
