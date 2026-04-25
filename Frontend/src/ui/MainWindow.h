//
// Created by Anmol on 05-04-2026.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QTimer>
#include <QTabWidget>
#include <QScrollArea>
#include <QDesktopServices>
#include <QUrl>
#include <QStackedWidget>
#include <QComboBox>
#include "VelocityTriangleWidget.h"
#include "ChatMentorWidget.h"
#include "ProfileExplorer3DWidget.h"
#include "ProfessorDashboard.h"
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendSolveRequest();
    void onReplyFinished(class QNetworkReply *reply);
    void sendMentorMessage(const QString &msg);
    void onMentorReplyFinished(class QNetworkReply *reply);
    void toggleTheme();
    void openLogin();
    void purchaseSemesterPass();
    void checkBackendHealth();
    void switchMode(int index);

private:
    QNetworkAccessManager *networkManager;

    // Backend Process Management
    QTimer *healthCheckTimer = nullptr;
    bool m_backendReady = false;
    int m_healthCheckAttempts = 0;
    void startBackendServer();
    void stopBackendServer();

    // UI Elements
    QTextEdit *nlpInputArea;
    QLineEdit *tokenInput;
    QPushButton *solveButton;
    QPushButton *loginButton;
    QPushButton *purchaseButton;
    QPushButton *themeToggleBtn;
    QLabel *resultsLabel;
    QLabel *freeUsesLabel;
    QLabel *backendStatusLabel;
    VelocityTriangleWidget *canvasWidget;
    QTabWidget *tabWidget;

    // Mentor Mode Elements
    QStackedWidget *mainStack;
    QComboBox *modeSelector;
    ChatMentorWidget *chatWidget;
    ProfileExplorer3DWidget *profile3DWidget;
    ProfessorDashboard *professorDashboard;

    bool m_isDarkMode = true;
    void setupUI();
    void applyTheme(bool dark);
};

#endif // MAINWINDOW_H