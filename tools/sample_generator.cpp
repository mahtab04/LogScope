#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "samples/generated.jsonl";
    std::ofstream output(path);
    if (!output) {
        std::cerr << "Cannot open: " << path << '\n';
        return 1;
    }
    const char* levels[] = {"INFO", "DEBUG", "WARN", "ERROR"};
    for (int i = 0; i < 25; ++i) {
        const nlohmann::json record = {
            {"timestamp", fmt::format("2026-10-18T10:{:02d}:00Z", i)},
            {"level", levels[i % 4]},
            {"source", "sample-generator"},
            {"message", fmt::format("Demo event {}", i + 1)}
        };
        output << record.dump() << '\n';
    }
    spdlog::info("Wrote 25 JSONL records to {}", path);
}
