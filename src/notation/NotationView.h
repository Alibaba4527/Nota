// Notation view for Nota.
// Paints the empty grand staff. The view does not own a score and takes no input yet.

#pragma once

#include <QWidget>

class QPaintEvent;

namespace nota {

class NotationView : public QWidget {
public:
    explicit NotationView(QWidget* parent = nullptr);

    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
};

} // namespace nota
