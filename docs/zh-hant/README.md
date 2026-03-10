# llmapi 中文文件

`llmapi` 是一個基於 C++23 模組的 LLM 客戶端函式庫。

語言：

- [English](../en/README.md)
- [简体中文](../zh/README.md)
- 繁體中文

## 內容導覽

- [快速開始](getting-started.md)
- [C++ API 參考](cpp-api.md)
- [範例](examples.md)
- [Provider 設定](providers.md)
- [進階用法](advanced.md)

## 核心能力

- `import mcpplibs.llmapi`
- 型別化訊息、工具與回應結構
- Provider 概念約束
- 內建 OpenAI / Anthropic 支援
- OpenAI 相容端點支援
- 對話儲存與還原

## 生產可用性

這個函式庫已可用於內部工具、原型專案與早期生產試用，但還不應被定義為「完整工業級基礎設施」。

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
