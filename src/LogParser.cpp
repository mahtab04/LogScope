#include "LogParser.h"

#include <nlohmann/json.hpp>

std::optional<LogRecord> LogParser::parseJsonLine(const QString& line) const {
    const auto json = nlohmann::json::parse(line.toStdString(), nullptr, false);
    if (!json.is_object()) {
        return std::nullopt;
    }
    for (const auto* field : {"timestamp", "level", "source", "message"}) {
        if (!json.contains(field) || !json.at(field).is_string()) {
            return std::nullopt;
        }
    }

    const auto stringField = [&json](const char* field) {
        return QString::fromStdString(json.at(field).get<std::string>());
    };
    LogRecord record;
    record.timestamp = QDateTime::fromString(stringField("timestamp"), Qt::ISODate);
    record.level = stringField("level");
    record.source = stringField("source");
    record.message = stringField("message");
    return record;
}
