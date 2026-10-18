# High-level architecture

```mermaid
flowchart TD
    Libraries["spdlog + fmt"] -.-> Generator["Sample Log Generator"]
    Generator --> File["JSONL / Log File"]
    Watcher["QFileSystemWatcher"] -.->|File changed| Reader["LogFileReader"]
    File --> Reader
    Reader -->|Raw log lines| Parser["LogParser"]
    JSON["nlohmann_json"] -.-> Parser
    Parser -->|LogRecord objects| Model["LogModel"]
    Model --> Proxy["LogFilterProxyModel"]
    Filters["Search / Severity UI"] --> Proxy
    Proxy --> Table["QTableView"]
    Table --> Window["MainWindow"]
```

The arrows reproduce the supplied diagram's flow. `MainWindow` hosts the table and manages user interactions. The [class diagram](class-diagram.md) also introduces `LogMonitor` to coordinate file watching and reading.
