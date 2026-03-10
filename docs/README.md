# llmapi Documentation

Current documentation for the `llmapi` C++23 module library.

## Contents

- [Getting Started](getting-started.md) - install, build, and first request
- [C++ API Guide](cpp-api.md) - types, providers, and `Client<P>`
- [Examples](examples.md) - chat, streaming, embeddings, and tool flows
- [Providers](providers.md) - OpenAI, Anthropic, and compatible endpoints
- [Advanced Usage](advanced.md) - persistence, async calls, and custom configuration

## What The Library Provides

- C++23 modules via `import mcpplibs.llmapi`
- Typed chat messages and multimodal content structs
- Provider concepts for sync, async, streaming, and embeddings
- Built-in OpenAI and Anthropic providers
- OpenAI-compatible endpoint support through configurable base URLs
- Conversation save/load helpers for local session persistence

## License

Apache-2.0 - see [LICENSE](../LICENSE)
