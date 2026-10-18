#include "LogParser.h"
#include "LogRecord.h"

#include <gtest/gtest.h>
#include <QString>
#include <nlohmann/json.hpp>

namespace {
nlohmann::json validObject() {
    return {{"timestamp", "2026-10-18T10:00:00Z"},
        {"level", "INFO"},
        {"source", "server"},
        {"message", "Server started"}};
}
}

TEST(LogParserTest, ParsesAllFields) {
    const LogParser parser;
    const auto record = parser.parseJsonLine(QString::fromStdString(validObject().dump()));
    ASSERT_TRUE(record.has_value());
    EXPECT_EQ(record->timestamp, QDateTime::fromString(
        QStringLiteral("2026-10-18T10:00:00Z"), Qt::ISODate));
    EXPECT_EQ(record->level, QStringLiteral("INFO"));
    EXPECT_EQ(record->source, QStringLiteral("server"));
    EXPECT_EQ(record->message, QStringLiteral("Server started"));
}

TEST(LogParserTest, RejectsInvalidJsonAndNonObjects) {
    const LogParser parser;
    const QString inputs[] = {
        QString(), QStringLiteral(" \t\r\n"), QStringLiteral("{broken}"),
        QStringLiteral("{\"timestamp\":"), QStringLiteral("[]"),
        QStringLiteral("null"), QStringLiteral("42"), QStringLiteral("true"),
        QStringLiteral("\"text\""), QStringLiteral("{}"),
        QString::fromStdString(validObject().dump() + " garbage"),
        QString::fromStdString(validObject().dump() + "\n" + validObject().dump())
    };
    for (const auto& line : inputs) {
        SCOPED_TRACE(line.toStdString());
        EXPECT_FALSE(parser.parseJsonLine(line).has_value());
    }
}

TEST(LogParserTest, RejectsMissingRequiredFields) {
    const LogParser parser;
    for (const auto* field : {"timestamp", "level", "source", "message"}) {
        SCOPED_TRACE(field);
        auto json = validObject();
        json.erase(field);
        EXPECT_FALSE(parser.parseJsonLine(QString::fromStdString(json.dump())).has_value());
    }
}

TEST(LogParserTest, RejectsNonStringFields) {
    const LogParser parser;
    const nlohmann::json wrongTypes[] = {
        nullptr, 42, true, nlohmann::json::array(), nlohmann::json::object()
    };
    for (const auto* field : {"timestamp", "level", "source", "message"}) {
        SCOPED_TRACE(field);
        for (const auto& value : wrongTypes) {
            SCOPED_TRACE(value.dump());
            auto json = validObject();
            json[field] = value;
            EXPECT_FALSE(parser.parseJsonLine(QString::fromStdString(json.dump())).has_value());
        }
    }
}

TEST(LogParserTest, PreservesUnicodeAndEscapes) {
    const LogParser parser;
    const auto record = parser.parseJsonLine(QString::fromUtf8(
        R"({"timestamp":"time","level":"CUSTOM","source":"服务","message":"Hello 🌍\n\t\"quoted\" \\path"})"));
    ASSERT_TRUE(record.has_value());
    EXPECT_FALSE(record->timestamp.isValid());
    EXPECT_EQ(record->level, QStringLiteral("CUSTOM"));
    EXPECT_EQ(record->source, QString::fromUtf8("服务"));
    EXPECT_EQ(record->message, QString::fromUtf8("Hello 🌍\n\t\"quoted\" \\path"));
}

TEST(LogParserTest, AcceptsExtraFieldsAndSurroundingWhitespace) {
    auto json = validObject();
    json["extra"] = {{"requestId", 123}};
    const LogParser parser;
    const auto record = parser.parseJsonLine(
        QString::fromStdString(" \t" + json.dump() + "\r\n"));
    ASSERT_TRUE(record.has_value());
    EXPECT_EQ(record->message, QStringLiteral("Server started"));
}

TEST(LogParserTest, AcceptsEmptyStrings) {
    const LogParser parser;
    const auto record = parser.parseJsonLine(QStringLiteral(
        R"({"timestamp":"","level":"","source":"","message":""})"));
    ASSERT_TRUE(record.has_value());
    EXPECT_TRUE(!record->timestamp.isValid());
    EXPECT_TRUE(record->level.isEmpty());
    EXPECT_TRUE(record->source.isEmpty());
    EXPECT_TRUE(record->message.isEmpty());
}

TEST(LogParserTest, PreservesStringWhitespace) {
    auto json = validObject();
    json["message"] = "  message  ";
    const LogParser parser;
    const auto record = parser.parseJsonLine(QString::fromStdString(json.dump()));
    ASSERT_TRUE(record.has_value());
    EXPECT_EQ(record->message, QStringLiteral("  message  "));
}

TEST(LogParserTest, RecoversAfterInvalidInput) {
    const LogParser parser;
    EXPECT_TRUE(!parser.parseJsonLine(QStringLiteral("{broken}")).has_value());
    const auto first = parser.parseJsonLine(QString::fromStdString(validObject().dump()));
    ASSERT_TRUE(first.has_value());
    auto next = validObject();
    next["message"] = "Next event";
    const auto second = parser.parseJsonLine(QString::fromStdString(next.dump()));
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(second->message, QStringLiteral("Next event"));
    EXPECT_EQ(first->message, QStringLiteral("Server started"));
}
