#include "MainWindow.h"
#include "LogFileReader.h"
#include "LogParser.h"
#include "LogModel.h"

#include <QComboBox>
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>

namespace {
QLabel* label(const QString& text, const char* name, QWidget* parent = nullptr) {
    auto* result = new QLabel(text, parent);
    result->setObjectName(QString::fromLatin1(name));
    return result;
}
QFrame* card(const QString& title, const QString& subtitle, QLabel*& value, const char* tone) {
    auto* frame = new QFrame;
    frame->setObjectName("card");
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->addWidget(label(title, "cardTitle"));
    value = label("0", tone);
    layout->addWidget(value);
    layout->addWidget(label(subtitle, "muted"));
    return frame;
}
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LogScope");
    resize(1320, 860);
    setMinimumSize(960, 680);
    auto* root = new QWidget;
    setCentralWidget(root);
    auto* shell = new QHBoxLayout(root);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);

    auto* sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(210);
    auto* navigation = new QVBoxLayout(sidebar);
    navigation->setContentsMargins(24, 30, 24, 26);
    navigation->setSpacing(14);
    navigation->addWidget(label("◈  LogScope", "brand"));
    navigation->addWidget(label("YOUR LOG WORKSPACE", "sidebarCaption"));
    navigation->addSpacing(26);
    auto* explorer = new QPushButton("▤   Log explorer");
    explorer->setObjectName("navActive");
    navigation->addWidget(explorer);
    connect(explorer, &QPushButton::clicked, this, [this] { table_->setFocus(); });
    auto* openSide = new QPushButton("＋   Open log file");
    openSide->setObjectName("navButton");
    connect(openSide, &QPushButton::clicked, this, &MainWindow::chooseFile);
    navigation->addWidget(openSide);
    navigation->addStretch();
    auto* hint = label("A clearer view of\nwhat’s happening.", "sidebarHint");
    navigation->addWidget(hint);
    navigation->addSpacing(18);
    navigation->addWidget(label("LOCAL FILES  ·  v0.1.0", "sidebarCaption"));
    shell->addWidget(sidebar);

    auto* content = new QWidget;
    auto* page = new QVBoxLayout(content);
    page->setContentsMargins(30, 28, 30, 20);
    page->setSpacing(18);
    auto* top = new QHBoxLayout;
    auto* heading = new QVBoxLayout;
    heading->setSpacing(5);
    heading->addWidget(label("WORKSPACE  /  EXPLORER", "eyebrow"));
    heading->addWidget(label("Log explorer", "title"));
    heading->addWidget(label("Find the signal in every event.", "muted"));
    top->addLayout(heading);
    top->addStretch();
    auto* themeSelector = new QComboBox;
    themeSelector->setObjectName("themeSelector");
    themeSelector->setAccessibleName("Color theme");
    themeSelector->setToolTip("Choose Dark or Light appearance");
    themeSelector->addItem("Dark", "dark");
    themeSelector->addItem("Light", "light");
    const auto savedTheme = QSettings().value("appearance/theme", "dark").toString();
    const int savedIndex = themeSelector->findData(savedTheme);
    themeSelector->setCurrentIndex(savedIndex >= 0 ? savedIndex : 0);
    applyTheme(themeSelector->currentData().toString());
    connect(themeSelector, &QComboBox::currentIndexChanged, this, [this, themeSelector] {
        applyTheme(themeSelector->currentData().toString());
    });
    top->addWidget(themeSelector);
    auto* reload = new QPushButton("↻  Reload");
    reload->setToolTip("Read the current file again");
    connect(reload, &QPushButton::clicked, this, [this] {
        if (path_.isEmpty()) chooseFile(); else openFile(path_);
    });
    top->addWidget(reload);
    auto* open = new QPushButton("＋  Open file");
    open->setObjectName("primary");
    connect(open, &QPushButton::clicked, this, &MainWindow::chooseFile);
    top->addWidget(open);
    page->addLayout(top);

    auto* cards = new QHBoxLayout;
    cards->setSpacing(14);
    cards->addWidget(card("TOTAL EVENTS", "Records in the current file", totalLabel_, "metric"));
    cards->addWidget(card("ERRORS", "Events that need attention", errorLabel_, "errorMetric"));
    cards->addWidget(card("WARNINGS", "Potential issues to investigate", warningLabel_, "warningMetric"));
    page->addLayout(cards);

    auto* panel = new QFrame;
    panel->setObjectName("panel");
    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(18, 16, 18, 14);
    panelLayout->setSpacing(14);
    auto* fileRow = new QHBoxLayout;
    fileLabel_ = label("No file selected", "fileName");
    fileLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    fileRow->addWidget(fileLabel_, 1);
    fileRow->addWidget(label("JSONL  ·  READ ONLY", "eyebrow"));
    panelLayout->addLayout(fileRow);
    model_ = new LogModel(this);
    model_->setDarkTheme(themeSelector->currentData().toString() == "dark");
    table_ = new QTableView;
    table_->setModel(model_);
    table_->setAccessibleName("Log events");
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->setShowGrid(false);
    table_->setWordWrap(false);
    table_->setTextElideMode(Qt::ElideRight);
    table_->verticalHeader()->hide();
    table_->verticalHeader()->setDefaultSectionSize(42);
    table_->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table_->horizontalHeader()->setHighlightSections(false);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setColumnWidth(LogModel::Timestamp, 250);
    table_->setColumnWidth(LogModel::Level, 90);
    table_->setColumnWidth(LogModel::Source, 130);
    panelLayout->addWidget(table_, 1);
    notice_ = label("Open a JSONL file to begin.  •  Ctrl+O", "muted");
    notice_->setWordWrap(true);
    panelLayout->addWidget(notice_);
    page->addWidget(panel, 1);
    page->addWidget(label("Your logs stay on this machine.   •   Timestamps shown in UTC", "muted"));
    shell->addWidget(content, 1);

    auto* openAction = new QAction(this);
    openAction->setShortcut(QKeySequence::Open);
    addAction(openAction);
    connect(openAction, &QAction::triggered, this, &MainWindow::chooseFile);

}

void MainWindow::chooseFile() {
    const auto path = QFileDialog::getOpenFileName(this, "Open log file", path_,
        "Log files (*.jsonl *.log *.txt);;All files (*)");
    if (!path.isEmpty()) openFile(path);
}

bool MainWindow::openFile(const QString& path) {
    LogFileReader reader;
    if (!reader.open(path)) {
        QMessageBox::warning(this, "Unable to open file", reader.errorString());
        return false;
    }
    const auto lines = reader.readNewLines();
    if (!reader.errorString().isEmpty()) {
        QMessageBox::warning(this, "Unable to read file", reader.errorString());
        return false;
    }
    QVector<LogRecord> records;
    int invalid = 0, errors = 0, warnings = 0;
    const LogParser parser;
    for (const auto& line : lines) {
        if (auto record = parser.parseJsonLine(line)) {
            const auto level = record->level.toUpper();
            errors += level == "ERROR" || level == "FATAL";
            warnings += level == "WARN" || level == "WARNING";
            records.push_back(std::move(*record));
        } else {
            ++invalid;
        }
    }
    path_ = path;
    model_->setRecords(std::move(records));
    fileLabel_->setText(QFileInfo(path).fileName());
    fileLabel_->setToolTip(QFileInfo(path).absoluteFilePath());
    totalLabel_->setText(QString::number(model_->rowCount()));
    errorLabel_->setText(QString::number(errors));
    warningLabel_->setText(QString::number(warnings));
    notice_->setText(QString("%1 invalid lines skipped.  •  Only newline-terminated records are loaded.  •  Reload to read changes.").arg(invalid));
    setWindowTitle(QFileInfo(path).fileName() + " — LogScope");
    return true;
}

bool MainWindow::applyTheme(const QString& name) {
    if (name != "dark" && name != "light") return false;
    QFile stylesheet(QString(":/resources/themes/%1.qss").arg(name));
    if (!stylesheet.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to load theme:" << stylesheet.errorString();
        return false;
    }
    // Scope the theme to the workspace. File/message dialogs are parented to
    // MainWindow and should retain a consistent system palette.
    centralWidget()->setStyleSheet(QString::fromUtf8(stylesheet.readAll()));
    if (model_) model_->setDarkTheme(name == "dark");
    QSettings().setValue("appearance/theme", name);
    return true;
}
