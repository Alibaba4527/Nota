// Notation view for Nota.
// Paints the grand staff from the document and turns clicks into quarter notes.

#include "notation/NotationView.h"

#include "model/Document.h"
#include "notation/Glyphs.h"
#include "notation/Layout.h"

#include <QFont>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>

#include <cmath>
#include <optional>

namespace nota {
namespace {

// Half a staff space is 7 px. Selection still uses a target at least 12 px across.
constexpr qreal kHitSlop = 12.0;
constexpr int kMaxStepsFromMiddle = 7;

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

void paintLedgerLines(QPainter& painter, const NoteLayout& note) {
    const int half = static_cast<int>(std::lround(kStaffSpace * 1.15));
    const int x = static_cast<int>(std::lround(note.head.x())) - half;
    for (qreal ledger : note.ledgerY) {
        const int y = static_cast<int>(std::lround(ledger));
        painter.fillRect(QRect(x, y, half * 2, static_cast<int>(kLineThickness)), kInk);
    }
}

struct StaffHit {
    int measure = 0;
    Staff staff = Staff::Treble;
    int steps = 0;
};

std::optional<int> measureAt(const GrandStaffLayout& layout, qreal x) {
    for (int measure = 0; measure < kMeasureCount; ++measure) {
        const qreal left = measureLeft(layout, measure);
        const qreal right = measureRight(layout, measure);
        if (x >= left && x < right) {
            return measure;
        }
    }
    return std::nullopt;
}

std::optional<StaffHit> staffHitAt(const GrandStaffLayout& layout, QPointF point) {
    const std::optional<int> measure = measureAt(layout, point.x());
    if (!measure) {
        return std::nullopt;
    }

    const qreal gapMid = (layout.treble.y[4] + layout.bass.y[0]) * 0.5;
    const Staff staff = point.y() <= gapMid ? Staff::Treble : Staff::Bass;
    const qreal middleY = staff == Staff::Treble ? layout.treble.y[2] : layout.bass.y[2];
    const int steps = static_cast<int>(std::lround((middleY - point.y()) / (kStaffSpace * 0.5)));
    if (steps > kMaxStepsFromMiddle || steps < -kMaxStepsFromMiddle) {
        return std::nullopt;
    }
    return StaffHit{*measure, staff, steps};
}

const NoteLayout* noteAt(const GrandStaffLayout& layout, QPointF point, const std::optional<StaffHit>& hit) {
    const NoteLayout* closest = nullptr;
    qreal best = kHitSlop;
    for (const NoteLayout& note : layout.notes) {
        const qreal dx = point.x() - note.head.x();
        const qreal dy = point.y() - note.head.y();
        const qreal distance = std::hypot(dx, dy);
        if (distance > best) {
            continue;
        }
        const bool samePlace = hit && note.staff == hit->staff && note.steps == hit->steps;
        if (hit && !samePlace) {
            continue;
        }
        best = distance;
        closest = &note;
    }
    return closest;
}

} // namespace

NotationView::NotationView(Document* document, QWidget* parent) : QWidget(parent), document_(document) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, kPaper);
    setPalette(pal);
    connect(document_, &Document::changed, this, qOverload<>(&QWidget::update));
}

QSize NotationView::sizeHint() const {
    return roundedSize(layoutGrandStaff(document_->score()));
}

QSize NotationView::minimumSizeHint() const {
    return sizeHint();
}

void NotationView::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), kPaper);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const GrandStaffLayout layout = layoutGrandStaff(document_->score());
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

    painter.setRenderHint(QPainter::Antialiasing, false);
    for (const NoteLayout& note : layout.notes) {
        paintLedgerLines(painter, note);
    }

    const Selection selection = document_->selection();
    for (const NoteLayout& note : layout.notes) {
        paintQuarterNote(painter, kInk, note.head, kStaffSpace, note.stemUp);
        const bool selected = !selection.empty() && selection.measure == note.measure && selection.staff == note.staff
            && selection.note == note.index;
        if (selected) {
            paintNoteSelection(painter, kSelection, note.head, kStaffSpace);
        }
    }
}

void NotationView::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    setFocus(Qt::MouseFocusReason);

    const GrandStaffLayout layout = layoutGrandStaff(document_->score());
    const QPointF point = event->position();
    const std::optional<StaffHit> hit = staffHitAt(layout, point);
    if (const NoteLayout* note = noteAt(layout, point, hit)) {
        document_->select(Selection{note->measure, note->staff, note->index});
        return;
    }
    if (!hit) {
        if (const NoteLayout* note = noteAt(layout, point, std::nullopt)) {
            document_->select(Selection{note->measure, note->staff, note->index});
        }
        return;
    }
    document_->appendQuarter(hit->measure, hit->staff, pitchOnStaff(hit->staff, hit->steps));
}

void NotationView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        document_->deleteSelection();
        return;
    }
    QWidget::keyPressEvent(event);
}

} // namespace nota
