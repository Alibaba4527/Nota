// Main window for Nota.
// Owns the document and the notation view. No menus or toolbar yet.

#include "ui/MainWindow.h"

#include "model/Document.h"
#include "notation/Layout.h"
#include "notation/NotationView.h"

#include <QPalette>

namespace nota {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Nota"));

    auto* document = new Document(this);
    auto* view = new NotationView(document, this);
    setCentralWidget(view);
    view->setFocus();

    QPalette pal = palette();
    pal.setColor(QPalette::Window, kPaper);
    setPalette(pal);

    const QSize hint = view->sizeHint();
    resize(hint.width() + 72, hint.height() + 160);
}

} // namespace nota
