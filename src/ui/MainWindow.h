// Main window for Nota.
// Owns the document and the notation view. No menus or toolbar yet.

#pragma once

#include <QMainWindow>

namespace nota {

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
};

} // namespace nota
