# LogScope

A lightweight Qt 6 desktop viewer for JSON Lines application logs, developed in C++20.

**Status:** early development / starter skeleton.

## Stack
- Fedora Linux, GCC, CMake, Ninja
- Qt 6 Widgets (installed from Fedora packages)
- Conan 2: fmt, spdlog, nlohmann_json
- GoogleTest for unit tests (provided by Conan)

## Build

```bash
sudo dnf install -y gcc-c++ cmake ninja-build qt6-qtbase-devel
conan profile detect   # once per machine, if no profile exists
conan install . -s build_type=Debug --build=missing
cmake --preset conan-debug
cmake --build build/Debug --parallel
ctest --test-dir build/Debug --output-on-failure
./build/Debug/logscope
```

Open `samples/application.jsonl` from the File menu to see starter records.

Optional sample generator:
```bash
./build/Debug/logscope_samplegen samples/generated.jsonl
```

VS Code Remote SSH: use **CMake: Select Configure Preset → conan-debug**.
If Conan-generated files are missing, re-run `conan install ...` before CMake Configure.

## Roadmap
- [x] Project bootstrap, Conan dependencies and build
- [x] Basic JSONL parsing and table display
- [ ] Search and severity filtering
- [ ] Follow appended file data without re-reading entire file
- [ ] Robust malformed-line reporting and tests
- [ ] Export filtered records
- [ ] Screenshots and release packaging

## License
MIT (see LICENSE). Third-party libraries retain their own licenses.
