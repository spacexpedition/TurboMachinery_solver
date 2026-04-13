//
// Created by Anmol on 05-04-2026.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "VelocityTriangleWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendSolveRequest();
    void onReplyFinished(class QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;

    // UI Elements
    QTextEdit *nlpInputArea;
    QPushButton *solveButton;
    QLabel *resultsLabel;
    VelocityTriangleWidget *canvasWidget;

    void setupUI();
};

#endif // MAINWINDOW_H