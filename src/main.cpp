// Process entry for Nota. Shows the main window and runs the Qt event loop.

#include "ui/MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Nota"));

    nota::MainWindow window;
    window.show();
    return app.exec();
}
