#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("LogScope"));
    QApplication::setApplicationName(QStringLiteral("LogScope"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));
    QApplication::setStyle("Fusion");
    MainWindow window;
    if (argc > 1) window.openFile(QString::fromLocal8Bit(argv[1]));
    window.show();
    return app.exec();
}
