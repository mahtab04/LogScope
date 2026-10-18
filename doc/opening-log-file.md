# Opening a log file

```mermaid
flowchart TD
    Open["User clicks Open File"] --> Select["QFileDialog selects path"]
    Select --> FileOpens{"File opens?"}
    FileOpens -->|No| Error["Show error message"]
    FileOpens -->|Yes| Read["Read JSONL lines"]
    Read --> Parse["Parse each line"]
    Parse --> Valid{"Valid record?"}
    Valid -->|Yes| Add["Add LogRecord to collection"]
    Valid -->|No| ParseError["Record parse error"]
    Add --> More{"More lines?"}
    ParseError --> More
    More -->|Yes| Parse
    More -->|No| Model["Update LogModel"]
    Model --> Proxy["Proxy filter refreshes"]
    Proxy --> Table["QTableView displays logs"]
    Table --> Status["Update status bar"]
```

If a file cannot be opened, show an error. Otherwise, parse each line, collect valid records, and record parse errors while continuing through the remaining lines. Once all lines have been processed, update the model, refresh the filtered table, and update the status bar.
