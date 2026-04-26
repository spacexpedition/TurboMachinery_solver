#include "VisualEffectsOverlay.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPolygonF>
#include <cstdlib>

VisualEffectsOverlay::VisualEffectsOverlay(QWidget *parent) : QWidget(parent) {
    // Ensure the widget fills its parent and doesn't block events
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
}

void VisualEffectsOverlay::setDarkMode(bool dark) {
    if (m_isDarkMode != dark) {
        m_isDarkMode = dark;
        generateShards(); // Regenerate shards with new colors
        update(); // Trigger a repaint
    }
}

void VisualEffectsOverlay::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    generateShards(); // Regenerate shards when size changes to cover new area
}

void VisualEffectsOverlay::generateShards() {
    m_shards.clear();
    int w = width();
    int h = height();
    if (w == 0 || h == 0) return;

    // Generate some random intersecting polygons to simulate glass shards
    // We'll create large shards traversing the screen
    srand(42); // Fixed seed for consistent shards per size

    for (int i = 0; i < 7; ++i) { // Number of shards
        Shard shard;
        QPolygonF poly;
        
        // Randomize 3-5 points for each shard spanning large areas
        int points = 3 + (rand() % 3);
        for(int j = 0; j < points; ++j) {
            poly << QPointF(rand() % (w + 200) - 100, rand() % (h + 200) - 100);
        }
        
        shard.path.addPolygon(poly);
        shard.path.closeSubpath();

        // Colors based on theme
        if (m_isDarkMode) {
            // Dark mode shards: extremely subtle white/blue
            shard.color = QColor(255, 255, 255);
            shard.opacity = 0.005 + (rand() % 15) / 1000.0; // 0.005 to 0.02
        } else {
            // Light mode shards: subtle white/gray
            shard.color = QColor(255, 255, 255);
            shard.opacity = 0.02 + (rand() % 40) / 1000.0; // 0.02 to 0.06
        }
        
        m_shards.append(shard);
    }
}

void VisualEffectsOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // 1. Draw "Water" Background Gradient
    if (m_isDarkMode) {
        // Deep water / fluid dark colors
        QLinearGradient bgGrad(0, 0, w, h);
        bgGrad.setColorAt(0.0, QColor(15, 23, 42));   // Slate 900
        bgGrad.setColorAt(0.5, QColor(30, 27, 75));   // Indigo 950
        bgGrad.setColorAt(1.0, QColor(15, 23, 42));
        painter.fillRect(rect(), bgGrad);

        // Add a soft radial "glow" or ripple center
        QRadialGradient radial(w * 0.7, h * 0.3, w * 0.8);
        radial.setColorAt(0, QColor(139, 92, 246, 30)); // Violet soft
        radial.setColorAt(1, Qt::transparent);
        painter.fillRect(rect(), radial);
    } else {
        // Light water / fluid light colors
        QLinearGradient bgGrad(0, 0, w, h);
        bgGrad.setColorAt(0.0, QColor(248, 250, 252)); // Slate 50
        bgGrad.setColorAt(0.5, QColor(224, 231, 255)); // Indigo 100
        bgGrad.setColorAt(1.0, QColor(241, 245, 249)); // Slate 100
        painter.fillRect(rect(), bgGrad);

        QRadialGradient radial(w * 0.3, h * 0.7, w * 0.8);
        radial.setColorAt(0, QColor(99, 102, 241, 30)); // Indigo soft
        radial.setColorAt(1, Qt::transparent);
        painter.fillRect(rect(), radial);
    }

    // 2. Draw "Glass Shards"
    for (const Shard &shard : qAsConst(m_shards)) {
        painter.setOpacity(shard.opacity);
        
        // Fill shard
        painter.fillPath(shard.path, shard.color);
        
        // Shard edge/highlight
        painter.setOpacity(shard.opacity * 1.2); // Very subtle edge
        QPen pen(shard.color, 0.5); // Thinner line
        painter.strokePath(shard.path, pen);
    }
}
