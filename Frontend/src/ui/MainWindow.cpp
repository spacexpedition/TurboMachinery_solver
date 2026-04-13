//
// Created by Anmol on 05-04-2026.
//

#include "MainWindow.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QDebug>
#include <QIcon>
#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Intelligent Turbomachinery Solver");
    setWindowIcon(QIcon(":/assets/logo.png"));
    setupUI();
    applyTheme(true); // Default to Dark Mode

    // Initialize the network manager for async HTTP calls
    networkManager = new QNetworkAccessManager(this);

    // Initialize backend process
    backendProcess = new QProcess(this);
    QString appDir = QCoreApplication::applicationDirPath();
    QString backendPath = QDir(appDir).filePath("api_server.exe");
    
    // In dev mode, maybe it's not there, but for release it will be
    if(QFile::exists(backendPath)) {
        qDebug() << "Starting packaged backend:" << backendPath;
        backendProcess->start(backendPath);
    } else {
        qDebug() << "api_server.exe not found at" << backendPath << ". Assuming manual/dev backend run.";
    }

    // Connect signals and slots
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::sendSolveRequest);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplyFinished);
}

MainWindow::~MainWindow() {
    if (backendProcess && backendProcess->state() == QProcess::Running) {
        backendProcess->terminate();
        if(!backendProcess->waitForFinished(3000)) {
            backendProcess->kill();
        }
    }
    // Qt's object tree handles deletion of parented widgets and networkManager
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // --- LEFT PANE (Controls) ---
    QWidget *leftPane = new QWidget();
    leftPane->setMinimumWidth(350);
    leftPane->setMaximumWidth(400);
    leftPane->setObjectName("LeftPane");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPane);
    leftLayout->setContentsMargins(15, 15, 15, 15);
    leftLayout->setSpacing(15);

    // Theme Toggle
    themeToggleBtn = new QPushButton("🔆 Toggle Light Mode");
    connect(themeToggleBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);

    QLabel *titleLabel = new QLabel("<b>Enter Natural Language Problem:</b>");
    
    nlpInputArea = new QTextEdit();
    nlpInputArea->setPlaceholderText("Paste textbook problem here... e.g. An axial turbine stage has a blade speed of 100 m/s...");
    nlpInputArea->setMinimumHeight(120);

    solveButton = new QPushButton("Solve Physics & Render Geometry");
    solveButton->setCursor(Qt::PointingHandCursor);

    resultsLabel = new QLabel("Results will appear here.");
    resultsLabel->setWordWrap(true);
    resultsLabel->setMinimumHeight(150);
    resultsLabel->setAlignment(Qt::AlignTop);

    leftLayout->addWidget(themeToggleBtn);
    leftLayout->addWidget(titleLabel);
    leftLayout->addWidget(nlpInputArea);
    leftLayout->addWidget(solveButton);
    leftLayout->addWidget(resultsLabel);
    leftLayout->addStretch(1);

    // --- RIGHT PANE (Canvas) ---
    canvasWidget = new VelocityTriangleWidget(this);

    // Assemble the panes
    mainLayout->addWidget(leftPane);
    mainLayout->addWidget(canvasWidget, 1); // Canvas takes remaining space stretch

    setCentralWidget(centralWidget);
}

void MainWindow::toggleTheme() {
    m_isDarkMode = !m_isDarkMode;
    applyTheme(m_isDarkMode);
    themeToggleBtn->setText(m_isDarkMode ? "🔆 Toggle Light Mode" : "🌙 Toggle Dark Mode");
}

void MainWindow::applyTheme(bool dark) {
    canvasWidget->setDarkMode(dark);

    if(dark) {
        // Dark Mode / Glassmorphic Aesthetic
        this->setStyleSheet(
            "QMainWindow { background-color: #121214; color: #FFFFFF; font-family: 'Segoe UI Variable', 'Inter', sans-serif; }"
            "QLabel { color: #E0E0E0; font-size: 14px; }"
            "QWidget#LeftPane { background-color: rgba(30, 30, 35, 0.8); border-radius: 12px; border: 1px solid rgba(255, 255, 255, 0.1); }"
            "QTextEdit { background-color: rgba(0, 0, 0, 0.4); color: #FFFFFF; padding: 10px; font-size: 13px; border-radius: 8px; border: 1px solid rgba(255, 255, 255, 0.1); }"
            "QPushButton { background-color: #0078D7; color: white; font-weight: bold; font-size: 14px; padding: 12px; border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: #005A9E; }"
            "QPushButton:disabled { background-color: #A0A0A0; }"
            "QLabel#ResultsLabel { background-color: rgba(0,0,0,0.3); padding: 10px; border-radius: 6px; font-family: monospace; border: 1px solid rgba(255,255,255,0.05); }"
        );
        resultsLabel->setObjectName("ResultsLabel");
    } else {
        // Light Mode Aesthetic
        this->setStyleSheet(
            "QMainWindow { background-color: #F8F9FA; color: #202124; font-family: 'Segoe UI Variable', 'Inter', sans-serif; }"
            "QLabel { color: #202124; font-size: 14px; }"
            "QWidget#LeftPane { background-color: #FFFFFF; border-radius: 12px; border: 1px solid #E0E0E0; }"
            "QTextEdit { background-color: #F1F3F4; color: #202124; padding: 10px; font-size: 13px; border-radius: 8px; border: 1px solid #DADCE0; }"
            "QPushButton { background-color: #1A73E8; color: white; font-weight: bold; font-size: 14px; padding: 12px; border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: #174EA6; }"
            "QPushButton:disabled { background-color: #BDBDBD; }"
            "QLabel#ResultsLabel { background-color: #F1F3F4; padding: 10px; border-radius: 6px; font-family: monospace; border: 1px solid #E0E0E0; }"
        );
        resultsLabel->setObjectName("ResultsLabel");
    }
}

void MainWindow::sendSolveRequest() {
    // Provide user feedback that the network call is happening
    solveButton->setEnabled(false);
    solveButton->setText("Solving (Calling Python Backend)...");

    // The backend is hosted locally by our Python process
    QUrl url("http://127.0.0.1:8080/api/solve");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject mainPayload;

    // Check if the user typed anything to trigger the NLP pipeline
    QString userInput = nlpInputArea->toPlainText().trimmed();
    if (!userInput.isEmpty()) {
        mainPayload["problem_text"] = userInput;
        mainPayload["known_variables"] = QJsonObject();
    } else {
        // If the box is empty, send a default textbook scenario so the app still works
        QJsonObject knowns;
        knowns["Cm1"] = 50.0;
        knowns["U1"] = 100.0;
        knowns["alpha1"] = 0.5236; // 30 degrees in radians
        mainPayload["known_variables"] = knowns;
    }

    // Serialize to JSON and send
    QJsonDocument doc(mainPayload);
    networkManager->post(request, doc.toJson());
}

void MainWindow::onReplyFinished(QNetworkReply *reply) {
    // Reset UI button state
    solveButton->setEnabled(true);
    solveButton->setText("Solve Physics & Render Geometry");

    // Handle connection failures (e.g., Python backend isn't running)
    if (reply->error() != QNetworkReply::NoError) {
        resultsLabel->setText("<b style='color:red;'>Connection Error:</b> Could not reach the local backend. Ensure FastAPI is running on 127.0.0.1:8080.");
        reply->deleteLater();
        return;
    }

    // Parse the JSON response from Python
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = doc.object();

    if (jsonObj["status"].toString() == "success") {
        // 1. Format the numerical data for the results label
        QString resultStr = "<b>NLP Engine:</b> " + jsonObj["nlp_engine_used"].toString() + "<br><b>Solved Parameters:</b><br>";

        QJsonObject results = jsonObj["results"].toObject();
        for (auto it = results.begin(); it != results.end(); ++it) {
            // Display numbers to 2 decimal places
            resultStr += QString("<b>%1</b>: %2 &nbsp;&nbsp;|&nbsp;&nbsp; ").arg(it.key()).arg(it.value().toDouble(), 0, 'f', 2);
        }
        resultsLabel->setText(resultStr);

        // 2. Pass the generated coordinates directly into the custom canvas widget
        canvasWidget->updateCoordinates(jsonObj["coordinates"].toObject());
    } else {
        // Handle mathematical errors (e.g., impossible geometry or missing variables)
        resultsLabel->setText("<b style='color:red;'>Backend Error:</b> " + jsonObj["message"].toString());
    }

    reply->deleteLater();
}