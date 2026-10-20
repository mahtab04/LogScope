#include "LogFilterProxyModel.h"
#include "LogModel.h"
#include <gtest/gtest.h>

class LogFilterTest : public testing::Test {
protected:
    LogModel model;
    LogFilterProxyModel proxy;
    void SetUp() override {
        model.setRecords({{{}, "INFO", "server", "Server started"},
                          {{}, "DEBUG", "network", "Client connected"},
                          {{}, "WARN", "network", "Latency increased"},
                          {{}, "ERROR", "db", "Query failed"},
                          {{}, "ERROR", "server", "Request failed [42]"}});
        proxy.setSourceModel(&model);
    }
};

TEST_F(LogFilterTest, EmptyFiltersPreserveOrderAndMapping) {
    ASSERT_EQ(proxy.rowCount(), 5);
    for (int row = 0; row < 5; ++row) {
        EXPECT_EQ(proxy.mapToSource(proxy.index(row, 0)).row(), row);
        EXPECT_EQ(proxy.index(row, LogModel::Message).data(), model.index(row, LogModel::Message).data());
    }
}

TEST_F(LogFilterTest, SearchesSourceAndMessageIgnoringCase) {
    proxy.setSearchText("NETWORK");
    ASSERT_EQ(proxy.rowCount(), 2);
    EXPECT_EQ(proxy.mapToSource(proxy.index(0, 0)).row(), 1);
    proxy.setSearchText("FAILED");
    EXPECT_EQ(proxy.rowCount(), 2);
    proxy.setSearchText("INFO");
    EXPECT_EQ(proxy.rowCount(), 0); // Severity is intentionally outside text search.
    proxy.setSearchText("[42]");
    EXPECT_EQ(proxy.rowCount(), 1); // Literal search, not a regular expression.
}

TEST_F(LogFilterTest, MatchesEachSeverityAndCombinesWithSearch) {
    for (const auto* level : {"DEBUG", "INFO", "WARN"}) {
        proxy.setSeverity(QString::fromLatin1(level));
        EXPECT_EQ(proxy.rowCount(), 1);
    }
    proxy.setSeverity("error");
    EXPECT_EQ(proxy.rowCount(), 2);
    proxy.setSearchText("server");
    ASSERT_EQ(proxy.rowCount(), 1);
    EXPECT_EQ(proxy.mapToSource(proxy.index(0, 0)).row(), 4);
    proxy.setSearchText("nothing matches");
    EXPECT_EQ(proxy.rowCount(), 0);
    proxy.setSearchText("");
    proxy.setSeverity("");
    EXPECT_EQ(proxy.rowCount(), 5);
    EXPECT_EQ(model.rowCount(), 5);
}

TEST_F(LogFilterTest, FiltersRemainActiveAcrossReloadsAndEmptySource) {
    proxy.setSeverity("ERROR");
    proxy.setSearchText("failed");
    model.setRecords({{{}, "INFO", "app", "failed"}, {{}, "ERROR", "app", "Failed again"}});
    ASSERT_EQ(proxy.rowCount(), 1);
    EXPECT_EQ(proxy.index(0, LogModel::Message).data().toString(), "Failed again");
    model.setRecords({});
    EXPECT_EQ(proxy.rowCount(), 0);
    proxy.setSourceModel(nullptr);
    EXPECT_EQ(proxy.rowCount(), 0);
}

TEST_F(LogFilterTest, PreservesSeverityColorsAndTooltips) {
    proxy.setSeverity("ERROR");
    const auto source = model.index(3, LogModel::Level);
    EXPECT_EQ(proxy.index(0, LogModel::Level).data(Qt::ForegroundRole), source.data(Qt::ForegroundRole));
    EXPECT_EQ(proxy.index(0, LogModel::Message).data(Qt::ToolTipRole), QStringLiteral("Query failed"));
}
