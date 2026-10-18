// Geometry for the grand staff.
// Positions are a pure function of the score and the constants below. The model does not store them.

#include "notation/Layout.h"

namespace nota {

namespace {

constexpr qreal kBraceBow = 16.0;
constexpr qreal kBraceThickness = 11.0;
constexpr qreal kClefGap = 6.0;
constexpr qreal kClefColumn = 40.0;
constexpr qreal kTimeGap = 4.0;
constexpr qreal kTimeWidth = 22.0;
constexpr qreal kAfterTime = 8.0;
constexpr qreal kFinalGap = 3.0;
constexpr qreal kFinalThick = 4.0;
constexpr qreal kInterStaffGap = 84.0;

// Keeps the onset-0 notehead off the barline. Later onsets use the same span.
constexpr qreal kNoteInset = 22.0;

qreal noteX(qreal left, qreal width, Duration onset, Duration beats) {
    const qreal fraction = (static_cast<qreal>(onset.num) * static_cast<qreal>(beats.den))
        / (static_cast<qreal>(onset.den) * static_cast<qreal>(beats.num));
    const qreal span = width - kNoteInset;
    return left + kNoteInset + fraction * span;
}

void appendVoice(GrandStaffLayout& layout, const Score& score, Staff staff) {
    const StaffLayout& lines = staff == Staff::Treble ? layout.treble : layout.bass;
    const qreal middleY = lines.y[2];
    const int measureCount = static_cast<int>(score.part.measures.size());
    const int count = measureCount < kMeasureCount ? measureCount : kMeasureCount;
    for (int measureIndex = 0; measureIndex < count; ++measureIndex) {
        const Measure& measure = score.part.measures[static_cast<size_t>(measureIndex)];
        const Voice& voice = voiceOf(measure, staff);
        const qreal left = measureLeft(layout, measureIndex);
        const qreal width = measureRight(layout, measureIndex) - left;
        for (int noteIndex = 0; noteIndex < static_cast<int>(voice.notes.size()); ++noteIndex) {
            const Note& note = voice.notes[static_cast<size_t>(noteIndex)];
            const int steps = stepsFromMiddle(staff, note.pitch);
            NoteLayout placed;
            placed.measure = measureIndex;
            placed.staff = staff;
            placed.index = noteIndex;
            placed.steps = steps;
            placed.stemUp = steps < 0;
            placed.head = QPointF(noteX(left, width, onsetBefore(voice, noteIndex), measure.beats),
                                  middleY - static_cast<qreal>(steps) * (kStaffSpace * 0.5));
            if (steps >= 6) {
                for (int ledger = 6; ledger <= steps; ledger += 2) {
                    placed.ledgerY.push_back(middleY - static_cast<qreal>(ledger) * (kStaffSpace * 0.5));
                }
            }
            if (steps <= -6) {
                for (int ledger = -6; ledger >= steps; ledger -= 2) {
                    placed.ledgerY.push_back(middleY - static_cast<qreal>(ledger) * (kStaffSpace * 0.5));
                }
            }
            layout.notes.push_back(placed);
        }
    }
}

} // namespace

qreal measureLeft(const GrandStaffLayout& layout, int measure) {
    if (measure <= 0) {
        return layout.contentStart;
    }
    return layout.barX[static_cast<size_t>(measure - 1)];
}

qreal measureRight(const GrandStaffLayout& layout, int measure) {
    return layout.barX[static_cast<size_t>(measure)];
}

GrandStaffLayout layoutGrandStaff(const Score& score) {
    GrandStaffLayout layout;

    const qreal trebleTop = kPageMargin;
    for (int line = 0; line < 5; ++line) {
        layout.treble.y[static_cast<size_t>(line)] = trebleTop + static_cast<qreal>(line) * kStaffSpace;
    }

    const qreal bassTop = layout.treble.y[4] + kInterStaffGap;
    for (int line = 0; line < 5; ++line) {
        layout.bass.y[static_cast<size_t>(line)] = bassTop + static_cast<qreal>(line) * kStaffSpace;
    }

    layout.systemTop = layout.treble.y[0];
    layout.systemBottom = layout.bass.y[4];
    layout.systemBarX = kPageMargin + kBraceBow + (kBraceThickness * 0.5);

    const qreal clefColumnLeft = layout.systemBarX + kClefGap;
    layout.trebleClefAnchor = QPointF(clefColumnLeft + 18.0, layout.treble.y[3]);
    layout.bassClefAnchor = QPointF(clefColumnLeft + 2.0, layout.bass.y[1]);
    layout.timeX = clefColumnLeft + kClefColumn + kTimeGap;
    layout.timeWidth = kTimeWidth;
    layout.contentStart = layout.timeX + layout.timeWidth + kAfterTime;

    for (int measure = 0; measure < kMeasureCount; ++measure) {
        layout.barX[static_cast<size_t>(measure)] =
            layout.contentStart + static_cast<qreal>(measure + 1) * kMeasureWidth;
    }

    layout.finalRight = layout.barX[kMeasureCount - 1] + kFinalGap + kFinalThick;
    layout.preferredSize = QSizeF(layout.finalRight + kPageMargin, layout.systemBottom + kPageMargin);

    appendVoice(layout, score, Staff::Treble);
    appendVoice(layout, score, Staff::Bass);
    return layout;
}

} // namespace nota
