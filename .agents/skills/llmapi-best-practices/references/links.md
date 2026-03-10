# llmapi 相关链接

## 仓库与主页

- GitHub 仓库：`https://github.com/mcpplibs/llmapi`
- README（英文）：`/home/speak/workspace/github/mcpplibs/llmapi/README.md`
- README（简中）：`/home/speak/workspace/github/mcpplibs/llmapi/README.zh.md`
- README（繁中）：`/home/speak/workspace/github/mcpplibs/llmapi/README.zh.hant.md`

## 文档入口

- 文档导航：`/home/speak/workspace/github/mcpplibs/llmapi/docs/README.md`
- 英文文档主页：`/home/speak/workspace/github/mcpplibs/llmapi/docs/en/README.md`
- 简中文档主页：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh/README.md`
- 繁中文档主页：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh-hant/README.md`

## 使用文档

- 英文入门：`/home/speak/workspace/github/mcpplibs/llmapi/docs/en/getting-started.md`
- 英文示例：`/home/speak/workspace/github/mcpplibs/llmapi/docs/en/examples.md`
- 英文 Provider：`/home/speak/workspace/github/mcpplibs/llmapi/docs/en/providers.md`
- 英文高级用法：`/home/speak/workspace/github/mcpplibs/llmapi/docs/en/advanced.md`
- 英文 C++ API：`/home/speak/workspace/github/mcpplibs/llmapi/docs/en/cpp-api.md`

- 简中入门：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh/getting-started.md`
- 简中示例：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh/examples.md`
- 简中 Provider：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh/providers.md`
- 简中高级用法：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh/advanced.md`
- 简中 C++ API：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh/cpp-api.md`

- 繁中入门：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh-hant/getting-started.md`
- 繁中示例：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh-hant/examples.md`
- 繁中 Provider：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh-hant/providers.md`
- 繁中高级用法：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh-hant/advanced.md`
- 繁中 C++ API：`/home/speak/workspace/github/mcpplibs/llmapi/docs/zh-hant/cpp-api.md`

## 示例源码

- `examples/hello_mcpp.cpp`
- `examples/basic.cpp`
- `examples/chat.cpp`
- `examples/xmake.lua`

## 生态与依赖

- mcpplibs：`https://github.com/mcpplibs`
- mcpplibs-index：`https://github.com/mcpplibs/mcpplibs-index`
- xlings：`https://github.com/d2learn/xlings`
- xmake：`https://xmake.io/`
- OpenAI API：`https://platform.openai.com/docs/api-reference`
- Anthropic API：`https://docs.anthropic.com/`

## 推荐回答时的关键事实

- 默认 OpenAI 风格入口：`Client(Config{...})`
- Anthropic 入口：`Client(AnthropicConfig{...})`
- 推荐并发模型：实例隔离，上层并发，不共享 `Client`
- 推荐错误处理：异常模式为主，自动重试建议由库使用者在上层实现
