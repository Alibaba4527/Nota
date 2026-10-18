// Painter paths for notation symbols.
// Clefs, the brace, and the quarter note (filled head and stem).
// Coordinates below are staff spaces with y growing downward, except the brace,
// which is already in pixels.

#include "notation/Glyphs.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>
#include <cmath>
#include <numbers>
#include <vector>

namespace nota {
namespace {

struct InkPoint {
    qreal x;
    qreal y;
    qreal width;
};

QPointF catmullRom(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3, qreal t) {
    const qreal t2 = t * t;
    const qreal t3 = t2 * t;
    return (2.0 * p1 + (-p0 + p2) * t + (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2
            + (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3)
        * 0.5;
}

QPainterPath ribbonPath(const std::vector<InkPoint>& controls) {
    QPainterPath path;
    if (controls.size() < 2) {
        return path;
    }

    struct Sample {
        QPointF point;
        qreal halfWidth;
    };

    std::vector<Sample> samples;
    const int count = static_cast<int>(controls.size());
    constexpr int kSteps = 12;
    samples.reserve(static_cast<size_t>(count * kSteps));

    for (int segment = 0; segment < count - 1; ++segment) {
        const InkPoint& p0 = controls[static_cast<size_t>(std::max(0, segment - 1))];
        const InkPoint& p1 = controls[static_cast<size_t>(segment)];
        const InkPoint& p2 = controls[static_cast<size_t>(segment + 1)];
        const InkPoint& p3 = controls[static_cast<size_t>(std::min(count - 1, segment + 2))];
        for (int step = 0; step < kSteps; ++step) {
            const qreal t = static_cast<qreal>(step) / static_cast<qreal>(kSteps);
            const QPointF point = catmullRom(QPointF(p0.x, p0.y), QPointF(p1.x, p1.y), QPointF(p2.x, p2.y),
                                             QPointF(p3.x, p3.y), t);
            const qreal width = p1.width + (p2.width - p1.width) * t;
            samples.push_back(Sample{point, width * 0.5});
        }
    }
    const InkPoint& last = controls.back();
    samples.push_back(Sample{QPointF(last.x, last.y), last.width * 0.5});

    std::vector<QPointF> left;
    std::vector<QPointF> right;
    left.reserve(samples.size());
    right.reserve(samples.size());

    QPointF previousNormal;
    bool hasPreviousNormal = false;
    for (size_t index = 0; index < samples.size(); ++index) {
        QPointF tangent;
        if (index == 0) {
            tangent = samples[1].point - samples[0].point;
        } else if (index + 1 == samples.size()) {
            tangent = samples[index].point - samples[index - 1].point;
        } else {
            tangent = samples[index + 1].point - samples[index - 1].point;
        }

        qreal length = std::hypot(tangent.x(), tangent.y());
        if (length < 1e-4) {
            tangent = hasPreviousNormal ? QPointF(-previousNormal.y(), previousNormal.x()) : QPointF(1.0, 0.0);
            length = std::hypot(tangent.x(), tangent.y());
        }

        QPointF normal(-tangent.y() / length, tangent.x() / length);
        if (hasPreviousNormal && QPointF::dotProduct(normal, previousNormal) < 0.0) {
            normal = -normal;
        }
        previousNormal = normal;
        hasPreviousNormal = true;

        const qreal half = samples[index].halfWidth;
        left.push_back(samples[index].point + normal * half);
        right.push_back(samples[index].point - normal * half);
    }

    path.moveTo(left.front());
    for (size_t index = 1; index < left.size(); ++index) {
        path.lineTo(left[index]);
    }
    for (size_t index = right.size(); index-- > 0;) {
        path.lineTo(right[index]);
    }
    path.closeSubpath();
    path.setFillRule(Qt::WindingFill);
    return path;
}

void fillRibbon(QPainter& painter, const QColor& ink, const std::vector<InkPoint>& controls, QPointF origin, qreal scale) {
    painter.save();
    painter.translate(origin);
    painter.scale(scale, scale);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(ink);
    painter.drawPath(ribbonPath(controls));
    painter.restore();
}

} // namespace

void paintBrace(QPainter& painter, const QColor& ink, qreal xRight, qreal yTop, qreal yBottom) {
    const qreal height = yBottom - yTop;
    const qreal mid = (yTop + yBottom) * 0.5;
    const qreal bow = 13.0;
    const qreal x = xRight;

    // A curly brace: tips on the system barline, middle point to the left.
    QPainterPath path;
    path.moveTo(x, yTop);
    path.cubicTo(x - bow * 0.35, yTop + height * 0.10, x - bow * 1.05, mid - height * 0.16, x - bow * 1.15,
                 mid - height * 0.04);
    path.quadTo(QPointF(x - bow * 1.45, mid), QPointF(x - bow * 1.15, mid + height * 0.04));
    path.cubicTo(x - bow * 1.05, mid + height * 0.16, x - bow * 0.35, yBottom - height * 0.10, x, yBottom);
    path.cubicTo(x - bow * 0.70, yBottom - height * 0.14, x - bow * 0.55, mid + height * 0.07, x - bow * 0.28,
                 mid + 6.0);
    path.quadTo(QPointF(x - bow * 0.08, mid), QPointF(x - bow * 0.28, mid - 6.0));
    path.cubicTo(x - bow * 0.55, mid - height * 0.07, x - bow * 0.70, yTop + height * 0.14, x, yTop);
    path.closeSubpath();
    path.setFillRule(Qt::WindingFill);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(ink);
    painter.drawPath(path);
    painter.restore();
}

void paintTrebleClef(QPainter& painter, const QColor& ink, QPointF gLine, qreal staffSpace) {
    // Spiral centered on the G line, then one stroke up through the top curl,
    // down the left side, and out the bottom hook.
    std::vector<InkPoint> controls;
    constexpr int kSpiralSamples = 18;
    constexpr qreal kTurns = 1.35;
    constexpr qreal kEnd = 20.0 * std::numbers::pi_v<qreal> / 180.0;
    const qreal start = kEnd - kTurns * 2.0 * std::numbers::pi_v<qreal>;
    controls.reserve(kSpiralSamples + 16);
    for (int i = 0; i < kSpiralSamples; ++i) {
        const qreal t = static_cast<qreal>(i) / static_cast<qreal>(kSpiralSamples - 1);
        const qreal theta = start + t * (kEnd - start);
        const qreal radius = 0.04 + t * 0.95;
        controls.push_back(InkPoint{radius * std::cos(theta), -radius * std::sin(theta), 0.05 + t * 0.26});
    }
    const InkPoint tail[] = {
        {0.72, -1.05, 0.32}, {0.92, -1.80, 0.30}, {0.70, -2.50, 0.26}, {0.22, -3.15, 0.22},
        {-0.35, -3.65, 0.18}, {-0.85, -3.72, 0.15}, {-1.02, -3.25, 0.14}, {-0.72, -2.85, 0.16},
        {-0.55, -2.35, 0.24}, {-0.62, -1.40, 0.28}, {-0.58, -0.45, 0.28}, {-0.48, 0.50, 0.26},
        {-0.22, 1.35, 0.24}, {0.18, 2.05, 0.22}, {0.58, 2.48, 0.18}, {1.00, 2.12, 0.14},
    };
    controls.insert(controls.end(), std::begin(tail), std::end(tail));
    fillRibbon(painter, ink, controls, gLine, staffSpace);
}

void paintBassClef(QPainter& painter, const QColor& ink, QPointF fLine, qreal staffSpace) {
    const std::vector<InkPoint> controls = {
        {0.02, -1.28, 0.10}, {0.48, -1.42, 0.28}, {1.15, -1.05, 0.46}, {1.52, -0.28, 0.56},
        {1.42, 0.48, 0.52},  {0.92, 1.08, 0.36},  {0.28, 0.92, 0.16},  {0.02, 0.58, 0.08},
    };
    fillRibbon(painter, ink, controls, fLine, staffSpace);

    painter.save();
    painter.translate(fLine);
    painter.scale(staffSpace, staffSpace);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(ink);
    constexpr qreal kDotRadius = 0.19;
    painter.drawEllipse(QPointF(2.12, -0.50), kDotRadius, kDotRadius);
    painter.drawEllipse(QPointF(2.12, 0.50), kDotRadius, kDotRadius);
    painter.restore();
}

void paintQuarterNote(QPainter& painter, const QColor& ink, QPointF head, qreal staffSpace, bool stemUp) {
    const qreal headW = staffSpace * 1.28;
    const qreal headH = staffSpace * 0.95;
    const qreal stemX = head.x() + (stemUp ? headW * 0.46 : -headW * 0.46);
    const qreal stemLength = staffSpace * 3.5;
    const qreal stemEndY = head.y() + (stemUp ? -stemLength : stemLength);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen stem(ink, 1.7);
    stem.setCapStyle(Qt::FlatCap);
    painter.setPen(stem);
    painter.drawLine(QPointF(stemX, head.y()), QPointF(stemX, stemEndY));

    painter.translate(head);
    painter.rotate(-18.0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(ink);
    painter.drawEllipse(QRectF(-headW * 0.5, -headH * 0.5, headW, headH));
    painter.restore();
}

void paintNoteSelection(QPainter& painter, const QColor& color, QPointF head, qreal staffSpace) {
    const qreal headW = staffSpace * 1.28 + 6.0;
    const qreal headH = staffSpace * 0.95 + 6.0;

    painter.save();
    painter.translate(head);
    painter.rotate(-18.0);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(color, 2.0);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QRectF(-headW * 0.5, -headH * 0.5, headW, headH));
    painter.restore();
}

} // namespace nota
