# Class responsibilities

| Class | Single responsibility |
| --- | --- |
| `LogRecord` | Represent one parsed log event |
| `LogParser` | Convert and validate one JSONL record |
| `LogFileReader` | Read log data from a file |
| `LogModel` | Store records and expose them to Qt Model/View |
| `LogFilterProxyModel` | Filter records by severity and search text |
| `MainWindow` | Manage UI and user interactions |
| `LogMonitor` | Detect file changes and request new records |
