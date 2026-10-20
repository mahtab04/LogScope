#pragma once

#include <QSortFilterProxyModel>

class LogFilterProxyModel : public QSortFilterProxyModel {
public:
    explicit LogFilterProxyModel(QObject* parent = nullptr);
    void setSearchText(const QString& text);
    // Empty severity shows every level; nonempty values match exactly, ignoring case.
    void setSeverity(const QString& severity);

protected:
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

private:
    QString searchText_;
    QString severity_;
};
