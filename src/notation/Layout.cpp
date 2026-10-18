// Geometry for the empty grand staff.
// Positions come from constants. Nothing here is stored on a score.

#include "notation/Layout.h"

namespace nota {

namespace {

// Room left of the system barline so the brace can bow outward.
constexpr qreal kBraceBow = 16.0;
constexpr qreal kBraceThickness = 11.0;

// Clef column, then the 4/4 numerals, then the empty measures.
constexpr qreal kClefGap = 6.0;
constexpr qreal kClefColumn = 40.0;
constexpr qreal kTimeGap = 4.0;
constexpr qreal kTimeWidth = 22.0;
constexpr qreal kAfterTime = 8.0;

constexpr qreal kFinalGap = 3.0;
constexpr qreal kFinalThick = 4.0;

// Treble and bass staves, plus air for clef tails and the middle-C zone.
constexpr qreal kInterStaffGap = 84.0;

} // namespace

GrandStaffLayout layoutGrandStaff() {
    GrandStaffLayout layout;

    const qreal trebleTop = kPageMargin;
    for (int line = 0; line < 5; ++line) {
        layout.treble.y[static_cast<size_t>(line)] =
            trebleTop + static_cast<qreal>(line) * kStaffSpace;
    }

    const qreal bassTop = layout.treble.y[4] + kInterStaffGap;
    for (int line = 0; line < 5; ++line) {
        layout.bass.y[static_cast<size_t>(line)] =
            bassTop + static_cast<qreal>(line) * kStaffSpace;
    }

    layout.systemTop = layout.treble.y[0];
    layout.systemBottom = layout.bass.y[4];

    layout.systemBarX = kPageMargin + kBraceBow + (kBraceThickness * 0.5);

    const qreal clefColumnLeft = layout.systemBarX + kClefGap;
    // Treble anchor is the G line (second line from the bottom).
    layout.trebleClefAnchor = QPointF(clefColumnLeft + 18.0, layout.treble.y[3]);
    // Bass anchor is the F line (second line from the top).
    layout.bassClefAnchor = QPointF(clefColumnLeft + 2.0, layout.bass.y[1]);

    layout.timeX = clefColumnLeft + kClefColumn + kTimeGap;
    layout.timeWidth = kTimeWidth;
    layout.contentStart = layout.timeX + layout.timeWidth + kAfterTime;

    for (int measure = 0; measure < kMeasureCount; ++measure) {
        layout.barX[static_cast<size_t>(measure)] =
            layout.contentStart + static_cast<qreal>(measure + 1) * kMeasureWidth;
    }

    layout.finalRight = layout.barX[kMeasureCount - 1] + kFinalGap + kFinalThick;

    const qreal width = layout.finalRight + kPageMargin;
    const qreal height = layout.systemBottom + kPageMargin;
    layout.preferredSize = QSizeF(width, height);
    return layout;
}

} // namespace nota
