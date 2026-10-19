#include "LogModel.h"
#include <QPersistentModelIndex>
#include <gtest/gtest.h>

namespace {
LogRecord event(const QString& message = QStringLiteral("Server started")) {
    return {QDateTime::fromString(QStringLiteral("2026-10-18T12:00:00+02:00"), Qt::ISODate),
            QStringLiteral("INFO"), QStringLiteral("server"), message};
}
}

TEST(LogModelTest, EmptyTableHasFourColumnsAndNoChildren) {
    LogModel model;
    EXPECT_EQ(model.rowCount(), 0);
    EXPECT_EQ(model.columnCount(), 4);
    model.setRecords({event()});
    EXPECT_EQ(model.rowCount(), 1);
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 0);
    EXPECT_EQ(model.columnCount(model.index(0, 0)), 0);
}

TEST(LogModelTest, DisplaysAllFieldsAndConvertsTimestampToUtc) {
    LogModel model;
    model.setRecords({event()});
    EXPECT_EQ(model.data(model.index(0, LogModel::Timestamp)).toString(), "2026-10-18T10:00:00.000Z");
    EXPECT_EQ(model.data(model.index(0, LogModel::Level)).toString(), "INFO");
    EXPECT_EQ(model.data(model.index(0, LogModel::Source)).toString(), "server");
    EXPECT_EQ(model.data(model.index(0, LogModel::Message)).toString(), "Server started");
}

TEST(LogModelTest, ProvidesHeadersAndRejectsUnsupportedRequests) {
    LogModel model;
    model.setRecords({event()});
    const QStringList headers{"Timestamp (UTC)", "Level", "Source", "Message"};
    for (int column = 0; column < LogModel::ColumnCount; ++column)
        EXPECT_EQ(model.headerData(column, Qt::Horizontal).toString(), headers[column]);
    EXPECT_EQ(model.headerData(0, Qt::Vertical).toInt(), 1);
    EXPECT_FALSE(model.headerData(-1, Qt::Horizontal).isValid());
    EXPECT_FALSE(model.headerData(4, Qt::Horizontal).isValid());
    EXPECT_FALSE(model.headerData(1, Qt::Vertical).isValid());
    EXPECT_FALSE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole).isValid());
    EXPECT_FALSE(model.data({}).isValid());
    EXPECT_FALSE(model.data(model.index(1, 0)).isValid());
    EXPECT_FALSE(model.data(model.index(0, 4)).isValid());
    EXPECT_FALSE(model.data(model.index(0, 0), Qt::DecorationRole).isValid());
    LogModel other;
    other.setRecords({event()});
    EXPECT_FALSE(model.data(other.index(0, 0)).isValid());
}

TEST(LogModelTest, ReplacementNotifiesViewAndInvalidatesOldIndexes) {
    LogModel model;
    model.setRecords({event(), event("second")});
    QPersistentModelIndex oldIndex(model.index(1, 0));
    int resets = 0;
    QObject::connect(&model, &QAbstractItemModel::modelReset, &model, [&] { ++resets; });
    model.setRecords({event("replacement")});
    EXPECT_EQ(resets, 1);
    EXPECT_FALSE(oldIndex.isValid());
    ASSERT_EQ(model.rowCount(), 1);
    EXPECT_EQ(model.data(model.index(0, LogModel::Message)).toString(), "replacement");
    model.setRecords({});
    EXPECT_EQ(resets, 2);
    EXPECT_EQ(model.rowCount(), 0);
}

TEST(LogModelTest, PreservesMessagesAndProvidesFullTooltips) {
    LogModel model;
    const auto message = QString::fromUtf8("服务 started\nsecond line, with \"quotes\"");
    model.setRecords({event(message)});
    const auto index = model.index(0, LogModel::Message);
    EXPECT_EQ(model.data(index).toString(), message);
    EXPECT_EQ(model.data(index, Qt::ToolTipRole).toString(), message);
    EXPECT_FALSE(model.flags(index).testFlag(Qt::ItemIsEditable));
    EXPECT_FALSE(model.setData(index, QStringLiteral("changed")));
    EXPECT_EQ(model.data(index).toString(), message);
}

TEST(LogModelTest, LabelsInvalidTimestampsAndKeepsFileOrder) {
    LogModel model;
    auto invalid = event("first");
    invalid.timestamp = {};
    model.setRecords({invalid, event("second")});
    EXPECT_EQ(model.data(model.index(0, LogModel::Timestamp)).toString(), "Invalid timestamp");
    EXPECT_EQ(model.data(model.index(0, LogModel::Message)).toString(), "first");
    EXPECT_EQ(model.data(model.index(1, LogModel::Message)).toString(), "second");
}

TEST(LogModelTest, ColorsSeverityAndRefreshesWhenThemeChanges) {
    LogModel model;
    QVector<LogRecord> records;
    for (const auto* level : {"INFO", "DEBUG", "warn", "WARNING", "ERROR", "FATAL", "TRACE", "CUSTOM"}) {
        auto record = event();
        record.level = QString::fromLatin1(level);
        records.push_back(record);
    }
    model.setRecords(records);
    const auto foreground = [&model](int row) {
        return model.data(model.index(row, LogModel::Level), Qt::ForegroundRole);
    };
    EXPECT_NE(foreground(0), foreground(1));
    EXPECT_NE(foreground(2), foreground(4));
    EXPECT_EQ(foreground(2), foreground(3));
    EXPECT_EQ(foreground(4), foreground(5));
    EXPECT_TRUE(foreground(6).isValid());
    EXPECT_FALSE(foreground(7).isValid());
    EXPECT_FALSE(model.data(model.index(0, LogModel::Message), Qt::ForegroundRole).isValid());
    const auto darkColor = foreground(0);
    int updates = 0;
    QObject::connect(&model, &QAbstractItemModel::dataChanged, &model,
        [&](const QModelIndex& first, const QModelIndex& last, const QList<int>& roles) {
            ++updates;
            EXPECT_EQ(first.column(), LogModel::Level);
            EXPECT_EQ(last.row(), model.rowCount() - 1);
            EXPECT_TRUE(roles.contains(Qt::ForegroundRole));
        });
    model.setDarkTheme(false);
    EXPECT_EQ(updates, 1);
    EXPECT_NE(foreground(0), darkColor);
    EXPECT_EQ(model.data(model.index(2, LogModel::Level)).toString(), "warn");
}
