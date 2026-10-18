// Geometry for the empty grand staff.
// Positions come from constants. Nothing here is stored on a score.

#pragma once

#include <QColor>
#include <QPointF>
#include <QSizeF>

#include <array>

namespace nota {

// Milestone 1 drawing constants.
inline constexpr qreal kStaffSpace = 14.0;
inline constexpr qreal kLineThickness = 1.0;
inline constexpr int kMeasureCount = 4;
inline constexpr qreal kMeasureWidth = 160.0;
inline constexpr qreal kPageMargin = 36.0;

inline const QColor kInk(0x1a, 0x1a, 0x1a);
inline const QColor kPaper(0xf7, 0xf4, 0xec);

// Five staff lines. Index 0 is the top line.
struct StaffLayout {
    std::array<qreal, 5> y{};
};

// Pixel geometry of one braced grand staff with four empty measures.
struct GrandStaffLayout {
    StaffLayout treble;
    StaffLayout bass;
    qreal systemTop = 0;
    qreal systemBottom = 0;
    qreal systemBarX = 0;
    qreal contentStart = 0;
    std::array<qreal, kMeasureCount> barX{};
    qreal finalRight = 0;
    QPointF trebleClefAnchor;
    QPointF bassClefAnchor;
    qreal timeX = 0;
    qreal timeWidth = 0;
    QSizeF preferredSize;
};

// Layout for the milestone 1 staff. A pure function of the constants above.
[[nodiscard]] GrandStaffLayout layoutGrandStaff();

} // namespace nota
