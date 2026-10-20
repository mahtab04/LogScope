#include "LogFilterProxyModel.h"
#include "LogModel.h"

LogFilterProxyModel::LogFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}

void LogFilterProxyModel::setSearchText(const QString& text) {
    if (searchText_ == text) return;
    searchText_ = text;
    invalidate();
}

void LogFilterProxyModel::setSeverity(const QString& severity) {
    if (severity_ == severity) return;
    severity_ = severity;
    invalidate();
}

bool LogFilterProxyModel::filterAcceptsRow(int row, const QModelIndex& parent) const {
    if (!sourceModel()) return false;
    const auto value = [this, row, &parent](int column) {
        return sourceModel()->index(row, column, parent).data().toString();
    };
    if (!severity_.isEmpty() && value(LogModel::Level).compare(severity_, Qt::CaseInsensitive) != 0)
        return false;
    return searchText_.isEmpty()
        || value(LogModel::Source).contains(searchText_, Qt::CaseInsensitive)
        || value(LogModel::Message).contains(searchText_, Qt::CaseInsensitive);
}
