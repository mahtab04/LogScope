#include "LogRecord.h"

#include <gtest/gtest.h>
#include <QString>

TEST(LogRecordTest, DefaultsToEmptyFields) {
    const LogRecord record{};
    EXPECT_TRUE(!record.timestamp.isValid());
    EXPECT_TRUE(record.level.isEmpty());
    EXPECT_TRUE(record.source.isEmpty());
    EXPECT_TRUE(record.message.isEmpty());
}

TEST(LogRecordTest, StoresAndCopiesFields) {
    LogRecord original{};
    original.timestamp = QDateTime::fromString(QStringLiteral("2026-10-18T10:00:00Z"), Qt::ISODate);
    original.level = QStringLiteral("INFO");
    original.source = QStringLiteral("server");
    original.message = QStringLiteral("Server started");

    LogRecord copy = original;
    EXPECT_EQ(copy.timestamp, original.timestamp);
    EXPECT_EQ(copy.level, original.level);
    EXPECT_EQ(copy.source, original.source);
    EXPECT_EQ(copy.message, original.message);

    copy.message = QStringLiteral("Changed");
    EXPECT_EQ(original.message, QStringLiteral("Server started"));
}
