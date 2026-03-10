# llmapi Documentation

Current documentation for the `llmapi` C++23 module library.

Language:

- English
- [简体中文](../zh/README.md)
- [繁體中文](../zh-hant/README.md)

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

## Production Readiness

The library is usable for internal tools, prototypes, and early production adoption, but it should not yet be described as fully industrial-grade infrastructure.

Required gaps before that claim:

- Unified error model across providers and transport
- Retry, backoff, timeout, and idempotency policy
- Request cancellation for long-running and streaming calls
- Logging, metrics, trace hooks, and request correlation
- Hardening of the custom HTTP/TLS transport layer
- Fault-injection, concurrency, and large-scale mock testing
- Stronger API compatibility and versioning guarantees
- More complete production configuration surface
- Explicit thread-safety and concurrency semantics
- Operational documentation for retries, keys, proxies, and failure handling

## License

Apache-2.0 - see [LICENSE](../../LICENSE)
