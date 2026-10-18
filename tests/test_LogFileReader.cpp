#include "LogFileReader.h"

#include <QTemporaryDir>
#include <gtest/gtest.h>

class LogFileReaderTest : public testing::Test {
protected:
    QTemporaryDir directory;
    LogFileReader reader;

    QString path(const char* name = "app.jsonl") const {
        return directory.filePath(QString::fromLatin1(name));
    }

    void write(const QByteArray& bytes, QIODevice::OpenMode mode = QIODevice::WriteOnly,
               const char* name = "app.jsonl") {
        ASSERT_TRUE(directory.isValid());
        QFile file(path(name));
        ASSERT_TRUE(file.open(mode));
        ASSERT_EQ(file.write(bytes), bytes.size());
    }
};

TEST_F(LogFileReaderTest, ReportsClosedAndInvalidPaths) {
    EXPECT_FALSE(reader.isOpen());
    EXPECT_TRUE(reader.readNewLines().isEmpty());
    EXPECT_FALSE(reader.errorString().isEmpty());
    EXPECT_FALSE(reader.reset());
    EXPECT_FALSE(reader.open(QString()));
    EXPECT_FALSE(reader.errorString().isEmpty());
    EXPECT_FALSE(reader.open(path()));
    EXPECT_FALSE(reader.isOpen());
    EXPECT_FALSE(reader.errorString().isEmpty());
}

TEST_F(LogFileReaderTest, PreservesJsonBlankLinesAndLineEndings) {
    write("{\"message\":\"hello, world\"}\r\n\nbroken JSON\n");
    ASSERT_TRUE(reader.open(path()));
    EXPECT_EQ(reader.readNewLines(),
              (QStringList{QStringLiteral("{\"message\":\"hello, world\"}"),
                           QString(), QStringLiteral("broken JSON")}));
    EXPECT_TRUE(reader.readNewLines().isEmpty());
    EXPECT_TRUE(reader.errorString().isEmpty());
}

TEST_F(LogFileReaderTest, ReadsAppendsAfterEndOfFile) {
    write("first\n");
    ASSERT_TRUE(reader.open(path()));
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("first")});
    EXPECT_TRUE(reader.readNewLines().isEmpty());
    write("second\n", QIODevice::WriteOnly | QIODevice::Append);
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("second")});
}

TEST_F(LogFileReaderTest, RetainsPartialUtf8AndCrLfUntilComplete) {
    const auto bytes = QString::fromUtf8("服务").toUtf8();
    write(bytes.left(1));
    ASSERT_TRUE(reader.open(path()));
    EXPECT_TRUE(reader.readNewLines().isEmpty());
    write(bytes.mid(1) + "\r", QIODevice::WriteOnly | QIODevice::Append);
    EXPECT_TRUE(reader.readNewLines().isEmpty());
    write("\n", QIODevice::WriteOnly | QIODevice::Append);
    EXPECT_EQ(reader.readNewLines(), QStringList{QString::fromUtf8("服务")});
}

TEST_F(LogFileReaderTest, ResetRereadsFileWithoutDuplicatingPendingBytes) {
    write("first\npartial");
    ASSERT_TRUE(reader.open(path()));
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("first")});
    ASSERT_TRUE(reader.reset());
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("first")});
    write("\n", QIODevice::WriteOnly | QIODevice::Append);
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("partial")});
}

TEST_F(LogFileReaderTest, SwitchAndCloseDiscardPendingData) {
    write("old partial");
    write("new\n", QIODevice::WriteOnly, "other.jsonl");
    ASSERT_TRUE(reader.open(path()));
    EXPECT_TRUE(reader.readNewLines().isEmpty());
    ASSERT_TRUE(reader.open(path("other.jsonl")));
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("new")});
    reader.close();
    EXPECT_FALSE(reader.isOpen());
    ASSERT_TRUE(reader.open(path("other.jsonl")));
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("new")});
    EXPECT_FALSE(reader.open(path("missing")));
    EXPECT_FALSE(reader.isOpen());
}

TEST_F(LogFileReaderTest, HandlesEmptyFileAndTruncation) {
    write("");
    ASSERT_TRUE(reader.open(path()));
    EXPECT_TRUE(reader.readNewLines().isEmpty());
    write("old record\npending", QIODevice::WriteOnly | QIODevice::Append);
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("old record")});
    write("new\n");
    EXPECT_EQ(reader.readNewLines(), QStringList{QStringLiteral("new")});
    EXPECT_TRUE(reader.readNewLines().isEmpty());
}
