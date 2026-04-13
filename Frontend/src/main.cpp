//
// Created by Anmol on 05-04-2026.
//
#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    // Initialize the core Qt application event loop
    QApplication app(argc, argv);

    // Set modern styling to fit the engineering application aesthetic
    QApplication::setStyle("Fusion");

    // Instantiate and configure the main window
    MainWindow window;
    window.resize(1000, 700);
    window.setWindowTitle("Intelligent Turbomachinery Solver");
    window.show();

    // Execute the application loop
    return app.exec();
}