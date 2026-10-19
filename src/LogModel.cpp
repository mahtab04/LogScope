#include "LogModel.h"
#include <QColor>
#include <utility>

LogModel::LogModel(QObject* parent) : QAbstractTableModel(parent) {}

int LogModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(records_.size());
}

int LogModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant LogModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.model() != this || index.row() < 0 ||
        index.row() >= records_.size() || index.column() < 0 ||
        index.column() >= ColumnCount) {
        return {};
    }
    const auto& record = records_.at(index.row());
    if (role == Qt::ForegroundRole && index.column() == Level) {
        const auto level = record.level.toUpper();
        if (level == "ERROR" || level == "FATAL" || level == "CRITICAL")
            return QColor(darkTheme_ ? "#ff8796" : "#b4233d");
        if (level == "WARN" || level == "WARNING")
            return QColor(darkTheme_ ? "#efc475" : "#875500");
        if (level == "INFO")
            return QColor(darkTheme_ ? "#75d9b4" : "#087453");
        if (level == "DEBUG")
            return QColor(darkTheme_ ? "#bba1f5" : "#7040b0");
        if (level == "TRACE")
            return QColor(darkTheme_ ? "#8bbded" : "#28639a");
        return {}; // Unknown levels use the normal theme text color.
    }
    if (role != Qt::DisplayRole && role != Qt::ToolTipRole) return {};

    switch (index.column()) {
    case Timestamp:
        return record.timestamp.isValid()
            ? record.timestamp.toUTC().toString(Qt::ISODateWithMs)
            : QStringLiteral("Invalid timestamp");
    case Level: return record.level;
    case Source: return record.source;
    case Message: return record.message;
    default: return {};
    }
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || section < 0) return {};
    if (orientation == Qt::Vertical)
        return section < rowCount() ? QVariant(section + 1) : QVariant();

    switch (section) {
    case Timestamp: return QStringLiteral("Timestamp (UTC)");
    case Level: return QStringLiteral("Level");
    case Source: return QStringLiteral("Source");
    case Message: return QStringLiteral("Message");
    default: return {};
    }
}

void LogModel::setRecords(QVector<LogRecord> records) {
    beginResetModel();
    records_ = std::move(records);
    endResetModel();
}

void LogModel::setDarkTheme(bool dark) {
    if (darkTheme_ == dark) return;
    darkTheme_ = dark;
    if (!records_.isEmpty())
        emit dataChanged(index(0, Level), index(rowCount() - 1, Level), {Qt::ForegroundRole});
}
