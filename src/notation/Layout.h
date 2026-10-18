// Geometry for the grand staff.
// Positions are a pure function of the score and the constants below. The model does not store them.

#pragma once

#include "model/Score.h"

#include <QColor>
#include <QPointF>
#include <QSizeF>

#include <array>
#include <vector>

namespace nota {

inline constexpr qreal kStaffSpace = 14.0;
inline constexpr qreal kLineThickness = 1.0;
inline constexpr int kMeasureCount = 4;
inline constexpr qreal kMeasureWidth = 160.0;
inline constexpr qreal kPageMargin = 36.0;

inline const QColor kInk(0x1a, 0x1a, 0x1a);
inline const QColor kPaper(0xf7, 0xf4, 0xec);
inline const QColor kSelection(0x1d, 0x4e, 0xd8);

// Five staff lines. Index 0 is the top line.
struct StaffLayout {
    std::array<qreal, 5> y{};
};

// One drawn note. steps is diatonic steps above the staff's middle line.
struct NoteLayout {
    int measure = 0;
    Staff staff = Staff::Treble;
    int index = 0;
    int steps = 0;
    QPointF head;
    bool stemUp = false;
    std::vector<qreal> ledgerY;
};

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
    std::vector<NoteLayout> notes;
};

[[nodiscard]] GrandStaffLayout layoutGrandStaff(const Score& score);

[[nodiscard]] qreal measureLeft(const GrandStaffLayout& layout, int measure);
[[nodiscard]] qreal measureRight(const GrandStaffLayout& layout, int measure);

} // namespace nota
