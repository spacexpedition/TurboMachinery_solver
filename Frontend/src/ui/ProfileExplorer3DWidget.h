#ifndef PROFILEEXPLORER3DWIDGET_H
#define PROFILEEXPLORER3DWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

// Qt3D includes
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>

class ProfileExplorer3DWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProfileExplorer3DWidget(QWidget *parent = nullptr);

private:
    Qt3DExtras::Qt3DWindow *view3D;
    QWidget *container3D;
    Qt3DCore::QEntity *rootEntity;
};

#endif // PROFILEEXPLORER3DWIDGET_H
