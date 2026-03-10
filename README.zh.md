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

## 生产可用性

`llmapi` 目前适合内部工具、原型项目和早期生产试用，但还不应直接视为完整工业级基础设施。

要达到那个标准，至少还需要补齐：

- 统一的 provider / 传输层错误模型
- 重试、退避、超时、幂等策略
- 长请求和流式请求的取消能力
- 日志、指标、trace hook、请求关联信息
- 自研 HTTP/TLS 传输层的进一步加固
- 故障注入、并发、Mock、大规模测试
- 更强的 API 兼容性与版本稳定性承诺
- 更完整的生产配置面
- 明确的线程安全和并发语义
- 面向运维的重试、密钥、代理、故障处理文档

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
add_requires("llmapi 0.0.2")

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
