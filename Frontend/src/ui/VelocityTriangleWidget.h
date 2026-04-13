//
// Created by Anmol on 05-04-2026.
//

#ifndef TURBOMACHINES_SOLVER_VELOCITYTRIANGLEWIDGET_H
#define TURBOMACHINES_SOLVER_VELOCITYTRIANGLEWIDGET_H
#ifndef VELOCITYTRIANGLEWIDGET_H
#define VELOCITYTRIANGLEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <QString>

class VelocityTriangleWidget : public QWidget {
    Q_OBJECT

public:
    explicit VelocityTriangleWidget(QWidget *parent = nullptr);

    // Slot or public method to receive new coordinates from MainWindow
    void updateCoordinates(const QJsonObject& coords);

protected:
    // Override the native Qt paint event to draw our custom graphics
    void paintEvent(QPaintEvent *event) override;

private:
    QJsonObject currentCoords;

    // Helper drawing functions
    void drawTriangle(QPainter& painter, const QJsonObject& triangleData, int xOffset, const QString& label);
    void drawVector(QPainter& painter, QPointF start, QPointF end, QColor color, const QString& label);
};

#endif // VELOCITYTRIANGLEWIDGET_H