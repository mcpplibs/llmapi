# Roadmap

`llmapi` is already usable for internal tools, prototypes, and early production experiments. The items below describe the main gaps before it should be treated as fully industrial-grade infrastructure.

## Production And Infrastructure Priorities

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
