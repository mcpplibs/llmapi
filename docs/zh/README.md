# llmapi 中文文档

`llmapi` 是一个基于 C++23 模块的 LLM 客户端库。

语言：

- [English](../en/README.md)
- 简体中文
- [繁體中文](../zh-hant/README.md)

## 内容导航

- [快速开始](getting-started.md)
- [C++ API 参考](cpp-api.md)
- [示例](examples.md)
- [Provider 配置](providers.md)
- [高级用法](advanced.md)

## 核心能力

- `import mcpplibs.llmapi`
- 类型化消息、工具和响应结构
- Provider 概念约束
- OpenAI / Anthropic 内置支持
- OpenAI 兼容端点支持
- 对话保存与恢复

## 生产可用性

这个库已经可以用于内部工具、原型项目和早期生产试用，但还不应被定义为“完整工业级基础设施”。

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
