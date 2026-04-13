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
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include "VelocityTriangleWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendSolveRequest();
    void onReplyFinished(class QNetworkReply *reply);
    void toggleTheme();

private:
    QProcess *backendProcess;
    QNetworkAccessManager *networkManager;

    // UI Elements
    QTextEdit *nlpInputArea;
    QPushButton *solveButton;
    QPushButton *themeToggleBtn;
    QLabel *resultsLabel;
    VelocityTriangleWidget *canvasWidget;

    bool m_isDarkMode = true;
    void setupUI();
    void applyTheme(bool dark);
};

#endif // MAINWINDOW_H