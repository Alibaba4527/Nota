// Painter paths for notation symbols.
// Clefs, the brace, and the quarter note (filled head and stem).

#pragma once

#include <QColor>
#include <QPointF>

class QPainter;

namespace nota {

void paintBrace(QPainter& painter, const QColor& ink, qreal xRight, qreal yTop, qreal yBottom);

// gLine is the point on the treble G line the clef curls around.
void paintTrebleClef(QPainter& painter, const QColor& ink, QPointF gLine, qreal staffSpace);

// fLine is the bass F line. The two dots sit in the spaces on either side of it.
void paintBassClef(QPainter& painter, const QColor& ink, QPointF fLine, qreal staffSpace);

// Filled notehead and stem. stemUp is true when the note sits below the middle line.
void paintQuarterNote(QPainter& painter, const QColor& ink, QPointF head, qreal staffSpace, bool stemUp);

// Blue ring around the selected notehead.
void paintNoteSelection(QPainter& painter, const QColor& color, QPointF head, qreal staffSpace);

} // namespace nota
