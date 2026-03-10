# llmapi

> 使用 C++23 模組建構的現代 LLM 客戶端

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Module](https://img.shields.io/badge/module-ok-green.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)
[![OpenAI Compatible](https://img.shields.io/badge/OpenAI-Compatible-green.svg)](https://platform.openai.com/docs/api-reference)

| [English](README.md) - [简体中文](README.zh.md) - 繁體中文 |
|:---:|
| [文件導覽](docs/README.md) - [繁體中文文件](docs/zh-hant/README.md) - [English Docs](docs/en/README.md) - [简体中文文档](docs/zh/README.md) |

`llmapi` 提供型別化的 `Client<Provider>` API，涵蓋聊天、串流輸出、embeddings、工具呼叫與對話持久化。預設別名 `Config` 對應 OpenAI 風格設定，常見情況下不需要顯式寫出 `openai::OpenAI(...)`。

## 特性

- C++23 模組：`import mcpplibs.llmapi`
- 強型別訊息、工具與回應結構
- 同步、非同步、串流聊天介面
- OpenAI Provider 支援 embeddings
- 支援儲存 / 載入對話歷史
- 可透過 `baseUrl` 存取 OpenAI 相容端點

## 生產可用性

`llmapi` 目前適合內部工具、原型專案與早期生產試用，但還不應直接視為完整工業級基礎設施。

要達到那個標準，至少還需要補齊：

- 統一的 provider / 傳輸層錯誤模型
- 重試、退避、逾時、冪等策略
- 長請求與串流請求的取消能力
- 日誌、指標、trace hook、請求關聯資訊
- 自研 HTTP/TLS 傳輸層的進一步加固
- 故障注入、並發、Mock、大規模測試
- 更強的 API 相容性與版本穩定性承諾
- 更完整的生產設定面
- 明確的執行緒安全與並發語義
- 面向維運的重試、金鑰、代理、故障處理文件

## 快速開始

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
    auto resp = client.chat("用兩句話解釋 C++23 模組的價值。");

    std::cout << resp.text() << '\n';
    return 0;
}
```

## Provider

- `Config`：`openai::Config` 的匯出別名，預設走 OpenAI 風格
- `openai::OpenAI`：OpenAI 聊天、串流、工具呼叫、embeddings
- `AnthropicConfig` / `anthropic::Anthropic`：Anthropic 聊天與串流

相容端點範例：

```cpp
auto client = Client(Config{
    .apiKey = std::getenv("DEEPSEEK_API_KEY"),
    .baseUrl = std::string(URL::DeepSeek),
    .model = "deepseek-chat",
});
```

## 建置與執行

```bash
xmake
xmake run hello_mcpp
xmake run basic
xmake run chat
```

## 套件管理使用

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

更多內容見 [docs/zh-hant/getting-started.md](docs/zh-hant/getting-started.md)、[docs/zh-hant/providers.md](docs/zh-hant/providers.md) 與 [docs/zh-hant/README.md](docs/zh-hant/README.md)。

## 授權

Apache-2.0，詳見 [LICENSE](LICENSE)
