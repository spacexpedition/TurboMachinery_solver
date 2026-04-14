//
// Created by Anmol on 05-04-2026.
//

#include "VelocityTriangleWidget.h"
#include <QPen>
#include <QFont>
#include <QPainterPath>

VelocityTriangleWidget::VelocityTriangleWidget(QWidget *parent) : QWidget(parent) {
    // Set up the default widget properties
    setMinimumHeight(350);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void VelocityTriangleWidget::updateCoordinates(const QJsonObject& coords) {
    currentCoords = coords;
    // Trigger a repaint event whenever new data arrives from Python
    update();
}

void VelocityTriangleWidget::setDarkMode(bool dark) {
    if(m_isDarkMode != dark) {
        m_isDarkMode = dark;
        update();
    }
}

void VelocityTriangleWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    // Enable antialiasing for smooth vector lines
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw a background based on current theme
    painter.fillRect(rect(), m_isDarkMode ? QColor(25, 25, 30) : QColor(245, 245, 250));

    // If no data has been received yet, show a placeholder
    if (currentCoords.isEmpty()) {
        painter.setPen(m_isDarkMode ? QColor(150, 150, 150) : QColor(100, 100, 100));
        QFont font = painter.font();
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, "Awaiting Physics Data...\nEnter variables above and click Solve.");
        return;
    }

    // Split the canvas in half: Inlet on the left, Outlet on the right
    int midPoint = width() / 2;
    QRect inletBounds(0, 0, midPoint, height());
    QRect outletBounds(midPoint, 0, width() - midPoint, height());

    drawTriangle(painter, currentCoords["inlet"].toObject(), inletBounds, "Inlet Station (1)");
    drawTriangle(painter, currentCoords["outlet"].toObject(), outletBounds, "Outlet Station (2)");

    // Draw vertical separator
    painter.setPen(QPen(m_isDarkMode ? QColor(255, 255, 255, 50) : QColor(0, 0, 0, 50), 2));
    painter.drawLine(midPoint, 0, midPoint, height());
}

void VelocityTriangleWidget::drawTriangle(QPainter& painter, const QJsonObject& data, const QRect& bounds, const QString& label) {
    if(data.isEmpty()) return;

    // Extract tip coordinates from the JSON structure
    QJsonArray cTip = data["c_tip"].toArray();
    QJsonArray uTip = data["u_tip"].toArray();

    double cx = cTip[0].toDouble();
    double cy = cTip[1].toDouble();
    double ux = uTip[0].toDouble();

    // Determine the mathematical bounding box of the triangle
    double minX = std::min({0.0, cx, ux});
    double maxX = std::max({0.0, cx, ux});
    double minY = 0.0;
    double maxY = std::max({0.0, cy});

    double mathWidth = maxX - minX;
    double mathHeight = maxY - minY;

    // Fallbacks to avoid divide by zero
    if (mathWidth < 1) mathWidth = 1;
    if (mathHeight < 1) mathHeight = 1;

    // Screen padding
    int padding = 40;
    int drawW = bounds.width() - 2 * padding;
    int drawH = bounds.height() - 2 * padding;

    if (drawW < 10) drawW = 10;
    if (drawH < 10) drawH = 10;

    // Scale to fit the bounds
    double scaleX = drawW / mathWidth;
    double scaleY = drawH / mathHeight;
    double scale = std::min(scaleX, scaleY) * 0.9; // 0.9 multiplier to ensure it doesn't touch the very edges

    // --- Draw Graph Sheet Background ---
    painter.save();
    painter.setClipRect(bounds);
    QPen gridPen(m_isDarkMode ? QColor(255, 255, 255, 15) : QColor(0, 0, 0, 15), 1, Qt::SolidLine);
    painter.setPen(gridPen);
    
    int gridSpacingPx = 40; // Pixels per grid square
    
    // Draw vertical lines
    for (int x = bounds.left(); x < bounds.right(); x += gridSpacingPx) {
        painter.drawLine(x, bounds.top(), x, bounds.bottom());
    }
    // Draw horizontal lines
    for (int y = bounds.bottom(); y > bounds.top(); y -= gridSpacingPx) {
        painter.drawLine(bounds.left(), y, bounds.right(), y);
    }
    painter.restore();

    // --- Calculate Points ---
    // Calculate the physical origin by offsetting MinX to be positive, and factoring in the bounds.left() and padding
    int originX = bounds.left() + padding + (-minX * scale);
    int originY = bounds.bottom() - padding; // Qt Y is inverted, so Y=0 is near bottom

    QPointF origin(originX, originY);
    QPointF cPoint(originX + cx * scale, originY - cy * scale);
    QPointF uPoint(originX + ux * scale, originY);

    // --- Draw Station Label ---
    painter.setPen(m_isDarkMode ? QColor(220, 220, 220) : QColor(40, 40, 40));
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(11);
    painter.setFont(font);
    painter.drawText(bounds.left() + 20, bounds.top() + 30, label);

    // --- Draw Scale Information ---
    font.setBold(false);
    font.setPointSize(9);
    painter.setFont(font);
    
    double unitsPerGrid = gridSpacingPx / scale;
    QString scaleText = QString("Scale: 1 square = %1 m/s").arg(unitsPerGrid, 0, 'f', 1);
    
    painter.setPen(m_isDarkMode ? QColor(150, 150, 150) : QColor(100, 100, 100));
    painter.drawText(bounds.left() + 20, bounds.bottom() - 15, scaleText);

    // --- Draw the 3 Vectors of the Velocity Triangle ---
    // C (Absolute Velocity) - Blue
    drawVector(painter, origin, cPoint, QColor(0, 150, 255), "C");

    // U (Blade Velocity) - Green
    drawVector(painter, origin, uPoint, QColor(0, 200, 100), "U");

    // W (Relative Velocity) - Red
    drawVector(painter, uPoint, cPoint, QColor(255, 100, 100), "W");
}

void VelocityTriangleWidget::drawVector(QPainter& painter, QPointF start, QPointF end, QColor color, const QString& label) {
    // Configure the pen for drawing lines with rounded caps
    QPen pen(color, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    // Draw the main line
    painter.drawLine(start, end);

    // --- Arrowhead drawing logic ---
    QLineF line(end, start); // Line from tip back to start for angle calculation
    double angle = std::atan2(-line.dy(), line.dx());

    double arrowSize = 10;

    // Calculate the two points of the arrowhead triangle
    QPointF arrowP1 = end - QPointF(sin(angle + M_PI / 3) * arrowSize,
                                    cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = end - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                    cos(angle + M_PI - M_PI / 3) * arrowSize);

    // Draw solid arrowhead
    QPainterPath arrowHead;
    arrowHead.moveTo(end);
    arrowHead.lineTo(arrowP1);
    arrowHead.lineTo(arrowP2);
    painter.fillPath(arrowHead, color);

    // --- Label drawing logic ---
    // Place the label near the midpoint of the vector
    QPointF mid((start.x() + end.x()) / 2, (start.y() + end.y()) / 2);
    painter.setPen(m_isDarkMode ? QColor(240, 240, 240) : QColor(20, 20, 20)); // Contrast text
    painter.drawText(mid.x() + 8, mid.y() - 8, label);
}
