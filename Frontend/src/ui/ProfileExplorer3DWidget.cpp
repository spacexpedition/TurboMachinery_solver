#include "ProfileExplorer3DWidget.h"
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <QColor>

ProfileExplorer3DWidget::ProfileExplorer3DWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    view3D = new Qt3DExtras::Qt3DWindow();
    view3D->defaultFrameGraph()->setClearColor(QColor(QRgb(0x1e1e23))); // Dark mode background
    
    container3D = QWidget::createWindowContainer(view3D);
    container3D->setMinimumSize(400, 400);
    layout->addWidget(container3D);

    // Create Root Entity
    rootEntity = new Qt3DCore::QEntity();

    // Create a basic Cylinder to represent a compressor rotor hub
    Qt3DCore::QEntity *rotorEntity = new Qt3DCore::QEntity(rootEntity);
    
    Qt3DExtras::QCylinderMesh *rotorMesh = new Qt3DExtras::QCylinderMesh();
    rotorMesh->setRadius(2.0f);
    rotorMesh->setLength(6.0f);
    rotorMesh->setRings(100);
    rotorMesh->setSlices(30);

    Qt3DExtras::QPhongMaterial *rotorMaterial = new Qt3DExtras::QPhongMaterial();
    rotorMaterial->setDiffuse(QColor(QRgb(0x94a3b8))); // Slate metallic color
    rotorMaterial->setAmbient(QColor(QRgb(0x334155)));

    Qt3DCore::QTransform *rotorTransform = new Qt3DCore::QTransform();
    rotorTransform->setRotationX(90.0f); // Lay it flat

    rotorEntity->addComponent(rotorMesh);
    rotorEntity->addComponent(rotorMaterial);
    rotorEntity->addComponent(rotorTransform);

    // Camera
    Qt3DRender::QCamera *cameraEntity = view3D->camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 5, 15.0f));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // Camera Controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(cameraEntity);
    camController->setLinearSpeed(50.0f);
    camController->setLookSpeed(180.0f);

    view3D->setRootEntity(rootEntity);
}
