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

void VelocityTriangleWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    // Enable antialiasing for smooth vector lines
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw a dark background for the modern CAD/engineering aesthetic
    painter.fillRect(rect(), QColor(25, 25, 30));

    // If no data has been received yet, show a placeholder
    if (currentCoords.isEmpty()) {
        painter.setPen(QColor(150, 150, 150));
        QFont font = painter.font();
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, "Awaiting Physics Data...\nEnter variables above and click Solve.");
        return;
    }

    // Split the canvas in half: Inlet on the left, Outlet on the right
    int midPoint = width() / 2;
    drawTriangle(painter, currentCoords["inlet"].toObject(), 50, "Inlet Station (1)");
    drawTriangle(painter, currentCoords["outlet"].toObject(), midPoint + 50, "Outlet Station (2)");
}

void VelocityTriangleWidget::drawTriangle(QPainter& painter, const QJsonObject& data, int xOffset, const QString& label) {
    // Extract tip coordinates from the JSON structure
    QJsonArray cTip = data["c_tip"].toArray();
    QJsonArray uTip = data["u_tip"].toArray();

    // Visual scale factor to make coordinates fit nicely on screen.
    // In a full production app, this would dynamically calculate based on widget width/height.
    double scale = 2.0;

    // Qt's Y-axis starts at 0 at the top and goes down.
    // We invert it so Y=0 is at the bottom of the widget.
    int baseY = height() - 80;

    // Map mathematical coordinates to Qt Screen Points
    QPointF origin(xOffset, baseY);
    QPointF cPoint(xOffset + cTip[0].toDouble() * scale, baseY - cTip[1].toDouble() * scale);
    QPointF uPoint(xOffset + uTip[0].toDouble() * scale, baseY);

    // Draw Station Label
    painter.setPen(QColor(220, 220, 220));
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(11);
    painter.setFont(font);
    painter.drawText(xOffset, 30, label);

    // Reset font for vector labels
    font.setBold(false);
    font.setPointSize(9);
    painter.setFont(font);

    // Draw the 3 Vectors of the Velocity Triangle
    // C (Absolute Velocity) - Blue
    drawVector(painter, origin, cPoint, QColor(0, 150, 255), "C");

    // U (Blade Velocity) - Green
    drawVector(painter, origin, uPoint, QColor(0, 200, 100), "U");

    // W (Relative Velocity) - Red
    // Note: W connects the tip of U to the tip of C
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
    painter.setPen(QColor(240, 240, 240)); // White text
    painter.drawText(mid.x() + 8, mid.y() - 8, label);
}
