// Main window for Nota.
// Milestone 1 owns the notation view only: no menus, toolbar, or document.

#pragma once

#include <QMainWindow>

namespace nota {

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
};

} // namespace nota
