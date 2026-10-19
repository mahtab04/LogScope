#pragma once
#include <QMainWindow>

class QLabel;
class QTableView;
class LogModel;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
    bool openFile(const QString& path);
private:
    void chooseFile();
    bool applyTheme(const QString& name);
    LogModel* model_ = nullptr;
    QTableView* table_;
    QLabel* fileLabel_;
    QLabel* totalLabel_;
    QLabel* errorLabel_;
    QLabel* warningLabel_;
    QLabel* notice_;
    QString path_;
};
