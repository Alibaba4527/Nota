// Painter paths for notation symbols.
// Milestone 1 draws the brace, treble clef, and bass clef.

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

} // namespace nota
