#pragma once
#include <QMainWindow>

class QLabel;
class QTableView;
class LogModel;
class LogFilterProxyModel;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
    bool openFile(const QString& path);
private:
    void chooseFile();
    void updateVisibleCount();
    bool applyTheme(const QString& name);
    LogModel* model_ = nullptr;
    LogFilterProxyModel* filter_;
    QTableView* table_;
    QLabel* visibleCount_;
    QLabel* fileLabel_;
    QLabel* totalLabel_;
    QLabel* errorLabel_;
    QLabel* warningLabel_;
    QLabel* notice_;
    QString path_;
};
