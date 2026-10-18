# Reading log files

`LogFileReader` reads raw UTF-8 lines. `LogParser` validates each line separately, so a blank or malformed record does not prevent later records from being processed.

```cpp
LogFileReader reader;
LogParser parser;
if (!reader.open(path)) {
    // Report reader.errorString().
    return;
}
for (const auto& line : reader.readNewLines()) {
    if (const auto record = parser.parseJsonLine(line)) {
        // Add *record to the model.
    } else {
        // Count or report the invalid line, then continue.
    }
}
```

- `open()` closes the previous file and clears pending bytes. Failed opens leave the reader closed.
- `readNewLines()` returns complete newline-terminated lines, preserving blank lines and removing LF or CRLF endings. It retains unfinished lines, including split UTF-8 characters, until a newline arrives. A final line without a newline is intentionally held for a later append.
- Call `readNewLines()` again when the monitor detects changes. Previously returned lines are not repeated.
- An empty result means no complete lines are available, or an error occurred. Check `errorString()` after each operation; an empty error string indicates success.
- `reset()` seeks to the beginning and clears pending bytes. It returns whether seeking succeeded.
- A file shrinking below the current byte position triggers a reset. Truncation followed by regrowth to the old size before polling cannot be detected by size alone.
- File replacement/rotation requires the monitor to reopen the path. This reader does not watch paths or detect replacement of the underlying file.
- Reads load the available bytes into memory; very large files or unterminated lines will need a bounded reading strategy later.
