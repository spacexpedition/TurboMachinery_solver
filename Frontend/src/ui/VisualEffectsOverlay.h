#ifndef VISUALEFFECTSOVERLAY_H
#define VISUALEFFECTSOVERLAY_H

#include <QWidget>
#include <QPainterPath>
#include <QList>

class VisualEffectsOverlay : public QWidget {
    Q_OBJECT
public:
    explicit VisualEffectsOverlay(QWidget *parent = nullptr);
    void setDarkMode(bool dark);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool m_isDarkMode = true;
    
    struct Shard {
        QPainterPath path;
        QColor color;
        double opacity;
    };
    
    QList<Shard> m_shards;
    void generateShards();
};

#endif // VISUALEFFECTSOVERLAY_H
