// Notation view for Nota.
// Paints the grand staff from the document and turns clicks into quarter notes.

#pragma once

#include <QWidget>

class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

namespace nota {

class Document;

class NotationView : public QWidget {
public:
    explicit NotationView(Document* document, QWidget* parent = nullptr);

    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    Document* document_;
};

} // namespace nota
