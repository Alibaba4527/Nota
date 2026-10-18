// Notation view for Nota.
// Paints the empty grand staff. The view does not own a score and takes no input yet.

#include "notation/NotationView.h"

#include "notation/Glyphs.h"
#include "notation/Layout.h"

#include <QFont>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>

#include <cmath>

namespace nota {
namespace {

QSize roundedSize(const GrandStaffLayout& layout) {
    return QSize(static_cast<int>(std::lround(layout.preferredSize.width())),
                 static_cast<int>(std::lround(layout.preferredSize.height())));
}

void paintStaffLines(QPainter& painter, const StaffLayout& staff, int x0, int x1) {
    const int width = x1 - x0;
    for (qreal yValue : staff.y) {
        const int y = static_cast<int>(std::lround(yValue));
        painter.fillRect(QRect(x0, y, width, static_cast<int>(kLineThickness)), kInk);
    }
}

void paintSystemBarline(QPainter& painter, int x, int top, int bottom) {
    painter.fillRect(QRect(x, top, static_cast<int>(kLineThickness), bottom - top + 1), kInk);
}

void paintClosingBarline(QPainter& painter, int x, int top, int bottom, bool finalBar) {
    const int height = bottom - top + 1;
    painter.fillRect(QRect(x, top, static_cast<int>(kLineThickness), height), kInk);
    if (finalBar) {
        painter.fillRect(QRect(x + 4, top, 4, height), kInk);
    }
}

void paintTimeSignature(QPainter& painter, const StaffLayout& staff, qreal x, qreal width) {
    QFont font(QStringLiteral("Liberation Serif"));
    font.setStyleHint(QFont::Serif);
    font.setBold(true);
    font.setPixelSize(static_cast<int>(std::lround(kStaffSpace * 2.4)));
    painter.setFont(font);
    painter.setPen(QPen(kInk));

    const QString numeral = QStringLiteral("4");
    const QRect ink = QFontMetrics(font).tightBoundingRect(numeral);
    const qreal drawX = x + width * 0.5 - (ink.x() + ink.width() * 0.5);
    const qreal inkCenterOffset = ink.y() + ink.height() * 0.5;
    const qreal upperCenter = (staff.y[0] + staff.y[2]) * 0.5;
    const qreal lowerCenter = (staff.y[2] + staff.y[4]) * 0.5;
    painter.drawText(QPointF(drawX, upperCenter - inkCenterOffset), numeral);
    painter.drawText(QPointF(drawX, lowerCenter - inkCenterOffset), numeral);
}

} // namespace

NotationView::NotationView(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, kPaper);
    setPalette(pal);
}

QSize NotationView::sizeHint() const {
    return roundedSize(layoutGrandStaff());
}

QSize NotationView::minimumSizeHint() const {
    return sizeHint();
}

void NotationView::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), kPaper);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const GrandStaffLayout layout = layoutGrandStaff();
    const int x0 = static_cast<int>(std::lround(layout.systemBarX));
    const int x1 = static_cast<int>(std::lround(layout.finalRight));
    const int top = static_cast<int>(std::lround(layout.systemTop));
    const int bottom = static_cast<int>(std::lround(layout.systemBottom));

    paintStaffLines(painter, layout.treble, x0, x1);
    paintStaffLines(painter, layout.bass, x0, x1);
    paintSystemBarline(painter, x0, top, bottom);

    for (size_t measure = 0; measure < layout.barX.size(); ++measure) {
        const int x = static_cast<int>(std::lround(layout.barX[measure]));
        const bool finalBar = measure + 1 == layout.barX.size();
        paintClosingBarline(painter, x, top, bottom, finalBar);
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    paintTimeSignature(painter, layout.treble, layout.timeX, layout.timeWidth);
    paintTimeSignature(painter, layout.bass, layout.timeX, layout.timeWidth);

    paintBrace(painter, kInk, layout.systemBarX, layout.systemTop, layout.systemBottom);
    paintTrebleClef(painter, kInk, layout.trebleClefAnchor, kStaffSpace);
    paintBassClef(painter, kInk, layout.bassClefAnchor, kStaffSpace);
}

} // namespace nota
