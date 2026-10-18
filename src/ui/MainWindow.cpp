// Main window for Nota.
// Milestone 1 owns the notation view only: no menus, toolbar, or document.

#include "ui/MainWindow.h"

#include "notation/Layout.h"
#include "notation/NotationView.h"

#include <QPalette>

namespace nota {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Nota"));

    auto* view = new NotationView(this);
    setCentralWidget(view);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, kPaper);
    setPalette(pal);

    const QSize hint = view->sizeHint();
    resize(hint.width() + 72, hint.height() + 160);
}

} // namespace nota
