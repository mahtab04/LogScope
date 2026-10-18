#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("LogScope"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    // Replace this temporary window with MainWindow when it is implemented.
    QMainWindow window;
    window.setWindowTitle(QApplication::applicationName());
    window.resize(1000, 650);

    auto* welcome = new QLabel(
        QStringLiteral("LogScope\n\nJSONL log viewer\nOpen-file and table views are coming next."),
        &window);
    welcome->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(welcome);
    window.statusBar()->showMessage(QStringLiteral("Ready"));
    window.show();

    return app.exec();
}
