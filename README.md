# RealTimePatternDetection

### ETW 概述與架構
#### ETW 架構 Event Tracing for Windows (ETW) 是 Windows 平台上一個高效能、低負擔的事件追蹤系統。其架構包含以下幾個主要部分：
- Event Providers:
  - 事件提供者是負責產生事件的源頭。它們可以是內建於操作系統的，也可以是應用程式自己定義的。提供者通過 EventWrite 或 TraceLoggingWrite API 來發送事件。
- Event Consumers:
  - 事件消費者是負責接收和處理事件的應用程式或工具。這些消費者可以是實時的監控工具、日誌分析工具等。
- Event Controllers:
  - 事件控制器用來啟動和停止事件提供者，並配置其事件追蹤會話。控制器一般使用 StartTrace、ControlTrace 等 API。
- Event Tracing Sessions:
  - 事件追蹤會話是事件追蹤的單位，由事件控制器啟動，用來收集和傳送事件。會話可以配置為將事件寫入文件或通過網路實時傳輸。
### ETW 串接
#### 串接 ETW 步驟：
- 啟動事件追蹤會話:
  - 使用 StartTrace API 來啟動一個新的事件追蹤會話。
- 啟動事件提供者:
  - 使用 EnableTrace API 啟動特定的事件提供者。
- 處理事件:
  - 實現一個事件處理器，使用 EventRecordCallback 函數來處理事件。這個函數會在每次事件發生時被調用。
- 停止事件追蹤會話:
  - 使用 StopTrace API 停止事件追蹤會話。

### ETW 使用限制
#### 在開發產品時，使用 ETW 需要考量以下限制：
- 效能問題:
    - 儘管 ETW 設計為高效能，但在高頻率事件產生的情況下，仍可能對系統性能產生影響。
- 安全性:
    - 某些敏感信息的追蹤可能需要特殊的權限，並且可能涉及到隱私問題。
- 資料量:
    - 大量的事件資料可能導致存儲和處理的負擔，需要合理的數據管理策略。
- 相容性:
    - 不同版本的 Windows 對 ETW 的支持和功能可能有所不同，需要考慮相容性問題。

## 程式
- **請以系統管理員執行Visual Studio，第一次開啟需要比較久的時間來安裝NuGet package(Google Test、Google Mock)**

- **套件管理使用vcpkg，我的path為C:\vcpkg，如果有問題可以去專案屬性中修改**

- **依賴krabsetw、nlohmann-json**
  ```
  vcpkg.exe install nlohmann-json:x64-windows
  vcpkg.exe install krabsetw:x64-windows
  ```

- 此解決方案中有兩個專案，一個為題目要求的程式，一個為測試專案，兩個皆需要系統管理員權限。

