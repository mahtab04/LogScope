#pragma once

#include "LogRecord.h"
#include <QAbstractTableModel>
#include <QVector>

// Owns parsed records and exposes a read-only table in file order.
class LogModel : public QAbstractTableModel {
public:
    enum Column { Timestamp, Level, Source, Message, ColumnCount };

    explicit LogModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    void setRecords(QVector<LogRecord> records);
    void setDarkTheme(bool dark);

private:
    QVector<LogRecord> records_;
    bool darkTheme_ = true;
};
