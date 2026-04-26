//
// Created by Anmol on 05-04-2026.
//

#include "MainWindow.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDebug>
#include <QIcon>
#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLineEdit>
#include <QDesktopServices>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Intelligent Turbomachinery Solver");
    setWindowIcon(QIcon(":/assets/logo.png"));
    setupUI();
    applyTheme(true); // Default to Dark Mode

    // Initialize the network manager for async HTTP calls
    networkManager = new QNetworkAccessManager(this);

    // Connect signals and slots
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::sendSolveRequest);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplyFinished);

    // Disable interactive controls until backend is confirmed alive
    solveButton->setEnabled(false);
    loginButton->setEnabled(false);

    // Auto-start the Python backend server
    startBackendServer();
}

MainWindow::~MainWindow() {
    stopBackendServer();
    // Qt's object tree handles deletion of parented widgets and networkManager
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *appLayout = new QHBoxLayout(centralWidget);
    appLayout->setContentsMargins(0, 0, 0, 0); // Sidebar handles its own margins
    appLayout->setSpacing(0);
    
    // Left Pane: Sidebar
    sidebarWidget = new SidebarWidget(this);
    connect(sidebarWidget, &SidebarWidget::modeSelected, this, &MainWindow::switchMode);
    connect(sidebarWidget, &SidebarWidget::themeToggled, this, &MainWindow::toggleTheme);
    appLayout->addWidget(sidebarWidget);

    // Right Pane: Main Stack
    mainStack = new QStackedWidget();
    appLayout->addWidget(mainStack, 1); // Stretch to fill remaining space

    // --- SOLVER MODE WIDGET (Index 0) ---
    QWidget *solverWidget = new QWidget();
    QHBoxLayout *solverLayout = new QHBoxLayout(solverWidget);
    solverLayout->setContentsMargins(15, 15, 15, 15);
    solverLayout->setSpacing(15);

    // --- LEFT PANE (Controls) ---
    QWidget *leftPane = new QWidget();
    leftPane->setMinimumWidth(350);
    leftPane->setMaximumWidth(400);
    leftPane->setObjectName("LeftPane");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPane);
    leftLayout->setContentsMargins(15, 15, 15, 15);
    leftLayout->setSpacing(15);

    // Theme Toggle removed from here (now in sidebar)
    
    purchaseButton = new QPushButton("🛒 Purchase Semester Pass (₹499)");
    purchaseButton->setCursor(Qt::PointingHandCursor);
    purchaseButton->setStyleSheet("background-color: #f59e0b; color: white; font-weight: bold; border-radius: 6px; padding: 10px;");
    connect(purchaseButton, &QPushButton::clicked, this, &MainWindow::purchaseSemesterPass);

    // Login & Token Area
    QLabel *loginLabel = new QLabel("<b>Authentication:</b>");
    QHBoxLayout *loginLayout = new QHBoxLayout();
    tokenInput = new QLineEdit();
    tokenInput->setPlaceholderText("Paste your Session Token here...");
    tokenInput->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Get Token (Login)");
    loginButton->setCursor(Qt::PointingHandCursor);
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::openLogin);
    
    loginLayout->addWidget(tokenInput);
    loginLayout->addWidget(loginButton);

    QLabel *titleLabel = new QLabel("<b>Enter Natural Language Problem:</b>");
    
    nlpInputArea = new QTextEdit();
    nlpInputArea->setPlaceholderText("Paste textbook problem here... e.g. An axial turbine stage has a blade speed of 100 m/s...");
    nlpInputArea->setMinimumHeight(120);

    solveButton = new QPushButton("Solve Physics & Render Geometry");
    solveButton->setCursor(Qt::PointingHandCursor);

    freeUsesLabel = new QLabel("Free Solves Left: 5", this);
    freeUsesLabel->setStyleSheet("color: #10b981; font-weight: bold; padding-top: 5px;");

    backendStatusLabel = new QLabel("⏳ Starting backend server...", this);
    backendStatusLabel->setStyleSheet("color: #f59e0b; font-weight: bold; padding: 8px; border-radius: 6px; background-color: rgba(245,158,11,0.1); border: 1px solid rgba(245,158,11,0.3);");
    backendStatusLabel->setAlignment(Qt::AlignCenter);

    leftLayout->addWidget(backendStatusLabel);
    leftLayout->addWidget(purchaseButton);
    leftLayout->addWidget(loginLabel);
    leftLayout->addLayout(loginLayout);
    leftLayout->addWidget(titleLabel);
    leftLayout->addWidget(nlpInputArea);
    leftLayout->addWidget(solveButton);
    leftLayout->addWidget(freeUsesLabel);
    leftLayout->addStretch(1);

    // --- RIGHT PANE (Tabs) ---
    tabWidget = new QTabWidget(this);
    
    // Tab 1: Velocity Canvas
    canvasWidget = new VelocityTriangleWidget(this);
    tabWidget->addTab(canvasWidget, "Velocity Diagrams");
    
    // Tab 2: Step-by-Step Solution
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    // Remove borders from scroll area
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *solutionContainer = new QWidget(scrollArea);
    QVBoxLayout *solutionLayout = new QVBoxLayout(solutionContainer);
    
    resultsLabel = new QLabel("Results will appear here. Please log in and send a problem text.", solutionContainer);
    resultsLabel->setWordWrap(true);
    resultsLabel->setAlignment(Qt::AlignTop);
    
    solutionLayout->addWidget(resultsLabel);
    solutionLayout->addStretch(1);
    scrollArea->setWidget(solutionContainer);
    
    tabWidget->addTab(scrollArea, "Step-by-Step Solution");

    // Assemble the panes for Solver Mode
    solverLayout->addWidget(leftPane);
    solverLayout->addWidget(tabWidget, 1); // Tab area takes remaining space stretch
    mainStack->addWidget(solverWidget);

    // --- MENTOR MODE WIDGET (Index 1) ---
    QWidget *mentorWidget = new QWidget();
    QHBoxLayout *mentorLayout = new QHBoxLayout(mentorWidget);
    mentorLayout->setContentsMargins(15, 15, 15, 15);
    mentorLayout->setSpacing(15);
    
    chatWidget = new ChatMentorWidget();
    chatWidget->setMinimumWidth(350);
    chatWidget->setMaximumWidth(400);
    
    profile3DWidget = new ProfileExplorer3DWidget();
    
    mentorLayout->addWidget(chatWidget);
    mentorLayout->addWidget(profile3DWidget, 1);
    mainStack->addWidget(mentorWidget);

    // --- PROFESSOR DASHBOARD (Index 2) ---
    professorDashboard = new ProfessorDashboard();
    mainStack->addWidget(professorDashboard);

    setCentralWidget(centralWidget);

    connect(chatWidget, &ChatMentorWidget::sendMessage, this, &MainWindow::sendMentorMessage);
}

void MainWindow::switchMode(int index) {
    mainStack->setCurrentIndex(index);
}

void MainWindow::toggleTheme() {
    m_isDarkMode = !m_isDarkMode;
    applyTheme(m_isDarkMode);
    sidebarWidget->setDarkMode(m_isDarkMode);
}

void MainWindow::applyTheme(bool dark) {
    canvasWidget->setDarkMode(dark);

    if(dark) {
        // Dark Mode / Glassmorphic Aesthetic
        this->setStyleSheet(
            "QMainWindow { background-color: #121214; color: #FFFFFF; font-family: 'Segoe UI Variable', 'Inter', sans-serif; }"
            "QLabel { color: #E0E0E0; font-size: 14px; }"
            "QWidget#LeftPane { background-color: rgba(30, 30, 35, 0.8); border-radius: 12px; border: 1px solid rgba(255, 255, 255, 0.1); }"
            "QTextEdit, QLineEdit { background-color: rgba(0, 0, 0, 0.4); color: #FFFFFF; padding: 10px; font-size: 13px; border-radius: 8px; border: 1px solid rgba(255, 255, 255, 0.1); }"
            "QPushButton { background-color: #0078D7; color: white; font-weight: bold; font-size: 14px; padding: 12px; border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: #005A9E; }"
            "QPushButton:disabled { background-color: #A0A0A0; }"
            "QTabWidget::pane { border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; background-color: rgba(30, 30, 35, 0.8); top: -1px; }"
            "QTabBar::tab { background: rgba(0, 0, 0, 0.3); color: #A0A0A0; padding: 10px 20px; border-top-left-radius: 8px; border-top-right-radius: 8px; border: 1px solid transparent; }"
            "QTabBar::tab:selected { background: rgba(30, 30, 35, 0.8); color: #FFFFFF; border: 1px solid rgba(255, 255, 255, 0.1); border-bottom-color: rgba(30, 30, 35, 0.8); }"
            "QTabBar::tab:hover:!selected { background: rgba(0, 0, 0, 0.5); }"
            "QScrollArea { background-color: transparent; border: none; }"
            "QScrollArea > QWidget > QWidget { background-color: transparent; }"
            "QLabel#ResultsLabel { background-color: transparent; padding: 10px; font-size: 14px; }"
        );
        resultsLabel->setObjectName("ResultsLabel");
        loginButton->setStyleSheet("background-color: #10b981; color: white;");
    } else {
        // Light Mode Aesthetic
        this->setStyleSheet(
            "QMainWindow { background-color: #F8F9FA; color: #202124; font-family: 'Segoe UI Variable', 'Inter', sans-serif; }"
            "QLabel { color: #202124; font-size: 14px; }"
            "QWidget#LeftPane { background-color: #FFFFFF; border-radius: 12px; border: 1px solid #E0E0E0; }"
            "QTextEdit, QLineEdit { background-color: #F1F3F4; color: #202124; padding: 10px; font-size: 13px; border-radius: 8px; border: 1px solid #DADCE0; }"
            "QPushButton { background-color: #1A73E8; color: white; font-weight: bold; font-size: 14px; padding: 12px; border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: #174EA6; }"
            "QPushButton:disabled { background-color: #BDBDBD; }"
            "QTabWidget::pane { border: 1px solid #E0E0E0; border-radius: 8px; background-color: #FFFFFF; top: -1px; }"
            "QTabBar::tab { background: #F1F3F4; color: #5F6368; padding: 10px 20px; border-top-left-radius: 8px; border-top-right-radius: 8px; border: 1px solid transparent; }"
            "QTabBar::tab:selected { background: #FFFFFF; color: #1A73E8; font-weight: bold; border: 1px solid #E0E0E0; border-bottom-color: #FFFFFF; }"
            "QTabBar::tab:hover:!selected { background: #E8EAED; }"
            "QScrollArea { background-color: transparent; border: none; }"
            "QScrollArea > QWidget > QWidget { background-color: transparent; }"
            "QLabel#ResultsLabel { background-color: transparent; padding: 10px; font-size: 14px; }"
        );
        resultsLabel->setObjectName("ResultsLabel");
        loginButton->setStyleSheet("background-color: #10b981; color: white;");
    }
}

void MainWindow::openLogin() {
    if (!m_backendReady) {
        resultsLabel->setText("<b style='color:red;'>Server Not Ready:</b> The backend is still starting up. Please wait a moment.");
        return;
    }
    // Open the local login page in the default user browser
    QDesktopServices::openUrl(QUrl("http://127.0.0.1:8080/login"));
}

void MainWindow::sendSolveRequest() {
    if (!m_backendReady) {
        resultsLabel->setText("<b style='color:red;'>Server Not Ready:</b> The backend is still starting up. Please wait a moment.");
        return;
    }
    if (tokenInput->text().trimmed().isEmpty()) {
        resultsLabel->setText("<b style='color:red;'>Authentication Error:</b> Please get a token and paste it into the Authentication box.");
        return;
    }

    // Provide user feedback that the network call is happening
    solveButton->setEnabled(false);
    solveButton->setText("Solving (Calling Cloud Backend)...");

    // Replace this with your actual Cloud Run URL for production!
    QUrl url("http://127.0.0.1:8080/api/solve");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // Add the Google Session Token
    QString bearerStr = "Bearer " + tokenInput->text().trimmed();
    request.setRawHeader("Authorization", bearerStr.toUtf8());

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

    // Handle connection failures (e.g., Cloud backend isn't running or network is down)
    if (reply->error() != QNetworkReply::NoError) {
        
        // Check for 401 Unauthorized
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
            resultsLabel->setText("<b style='color:red;'>Authentication Error:</b> Invalid or expired token. Please Login again.");
        } else {
            resultsLabel->setText("<b style='color:red;'>Connection Error:</b> Could not reach the cloud proxy backend. Check your network.");
        }
        
        reply->deleteLater();
        return;
    }

    // Parse the JSON response from Python
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = doc.object();

    if (jsonObj["status"].toString() == "success") {
        QString usesLeft = jsonObj["free_uses_left"].toString();
        freeUsesLabel->setText("Free Solves Left: " + usesLeft);
        
        bool isPaywalled = jsonObj["paywalled"].toBool();
        
        if (isPaywalled) {
            QString lockedStr = "<div style='font-family: sans-serif; text-align: center; padding: 20px;'>";
            lockedStr += "<h2 style='color:#ef4444;'>🔒 PREMIUM SOLVER LOCKED</h2>";
            lockedStr += "<p style='font-size:14px;'>" + jsonObj["message"].toString() + "</p>";
            lockedStr += "<br><p><b>Here is the final numerical answer:</b></p>";
            
            QJsonObject results = jsonObj["results"].toObject();
            for (auto it = results.begin(); it != results.end(); ++it) {
                lockedStr += QString("<b>%1</b>: %2 <br>").arg(it.key()).arg(it.value().toDouble(), 0, 'f', 3);
            }
            
            lockedStr += "<br><p style='color:#f59e0b;'>Click the <b>🛒 Purchase Semester Pass</b> button to unlock Step-by-Step solutions and Dynamic Geometry graphics!</p>";
            lockedStr += "</div>";
            
            resultsLabel->setText(lockedStr);
            canvasWidget->updateCoordinates(QJsonObject()); // Clear geometry
            tabWidget->setCurrentIndex(1); // Force switch to solution to show lock
        } else {
            QString resultStr = "<div style='font-family: sans-serif; line-height: 1.4;'>";
            resultStr += "<b>NLP Engine:</b> " + jsonObj["nlp_engine_used"].toString() + "<hr>";

            QJsonArray steps = jsonObj["steps"].toArray();
            for(int i = 0; i < steps.size(); ++i) {
                QJsonObject step = steps[i].toObject();
                if (step["type"].toString() == "header") {
                    resultStr += "<br><b style='color:#0ea5e9; font-size:15px;'>" + step["title"].toString() + "</b><br>";
                    resultStr += step["content"].toString() + "<br>";
                } else if (step["type"].toString() == "solve_step") {
                    resultStr += "<div style='margin-left: 10px; margin-top: 10px; padding: 5px; border-left: 2px solid #475569;'>";
                    resultStr += "<b style='color:#f59e0b;'>" + step["title"].toString() + "</b><br>";
                    resultStr += "<span style='color:#94a3b8;'>Formula: </span><span style='font-family:monospace; background:rgba(0,0,0,0.2); padding: 2px;'>" + step["formula"].toString() + "</span><br>";
                    resultStr += "<span style='color:#94a3b8;'>Substitution: </span><span style='font-family:monospace; background:rgba(0,0,0,0.2); padding: 2px;'>" + step["substitution"].toString() + "</span><br>";
                    resultStr += "<b style='color:#10b981; font-size:14px; mt-1;'>Result: " + step["result_var"].toString() + " = " + QString::number(step["result_val"].toDouble(), 'f', 3) + "</b><br>";
                    resultStr += "</div>";
                }
            }
            resultStr += "</div>";
            resultsLabel->setText(resultStr);

            // Pass the generated coordinates directly into the custom canvas widget
            canvasWidget->updateCoordinates(jsonObj["coordinates"].toObject());
            tabWidget->setCurrentIndex(1);
        }
    } else {
        // Handle mathematical errors (e.g., impossible geometry or missing variables)
        resultsLabel->setText("<b style='color:red;'>Backend Error:</b> " + jsonObj["message"].toString());
    }

    reply->deleteLater();
}

void MainWindow::sendMentorMessage(const QString &msg) {
    if (!m_backendReady) {
        chatWidget->addMessage("System", "<b style='color:red;'>Server Not Ready.</b>", true);
        return;
    }
    
    QUrl url("http://127.0.0.1:8080/api/mentor/chat");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QString bearerStr = "Bearer " + tokenInput->text().trimmed();
    request.setRawHeader("Authorization", bearerStr.toUtf8());

    QJsonObject payload;
    payload["message"] = msg;
    QJsonDocument doc(payload);
    
    QNetworkReply *reply = networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onMentorReplyFinished(reply);
    });
}

void MainWindow::onMentorReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        chatWidget->addMessage("System", "<b style='color:red;'>Connection Error.</b>", true);
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = doc.object();

    if (jsonObj["status"].toString() == "success") {
        chatWidget->addMessage("Mentor", jsonObj["reply"].toString(), true);
    } else {
        chatWidget->addMessage("System", "<b style='color:red;'>Mentor Error:</b> " + jsonObj["reply"].toString(), true);
    }

    reply->deleteLater();
}

void MainWindow::purchaseSemesterPass() {
    // Open the external portal for Razorpay mock integration
    // In production, this would route to an authenticated API on the backend that returns a Razorpay Checkout Session URL properly.
    QDesktopServices::openUrl(QUrl("https://rzp.io/l/mock-test-link"));
}

// =====================================================================
// Backend Server Auto-Start Infrastructure
// =====================================================================

void MainWindow::startBackendServer() {
    QString appDirPath = QCoreApplication::applicationDirPath();

    // ── Strategy 1: Look for the PyInstaller-bundled backend_server.exe ──
    //    This is the installed / release path. The exe sits next to the GUI exe.
    QString bundledExe = QDir(appDirPath).absoluteFilePath("backend_server.exe");
    if (QFileInfo::exists(bundledExe)) {
        qDebug() << "[Backend] Launching bundled backend_server.exe:" << bundledExe;
        bool started = QProcess::startDetached(bundledExe, QStringList());
        if (!started) {
            backendStatusLabel->setText("❌ Backend Launch Failed");
            backendStatusLabel->setStyleSheet("color: #ef4444; font-weight: bold; padding: 8px; border-radius: 6px; background-color: rgba(239,68,68,0.1); border: 1px solid rgba(239,68,68,0.3);");
            resultsLabel->setText("<b style='color:red;'>Fatal Error:</b> Found backend_server.exe but could not start it.");
            return;
        }
    } else {
        // ── Strategy 2: Dev mode – walk up directories to find the .bat script ──
        QDir searchDir(appDirPath);
        QString scriptPath;
        for (int i = 0; i < 6; ++i) {
            scriptPath = searchDir.absoluteFilePath("start_backend_server.bat");
            if (QFileInfo::exists(scriptPath)) {
                qDebug() << "[Backend] Dev mode – found startup script:" << scriptPath;
                break;
            }
            scriptPath.clear();
            if (!searchDir.cdUp()) break;
        }

        if (scriptPath.isEmpty()) {
            backendStatusLabel->setText("❌ Backend Not Found");
            backendStatusLabel->setStyleSheet("color: #ef4444; font-weight: bold; padding: 8px; border-radius: 6px; background-color: rgba(239,68,68,0.1); border: 1px solid rgba(239,68,68,0.3);");
            resultsLabel->setText("<b style='color:red;'>Fatal Error:</b> Could not locate 'backend_server.exe' or 'start_backend_server.bat'.");
            return;
        }

        bool started = QProcess::startDetached("cmd.exe", QStringList() << "/c" << scriptPath);
        if (!started) {
            qDebug() << "[Backend] Failed to start batch script detached.";
        }
    }

    // Start polling the health endpoint every 1 second
    m_healthCheckAttempts = 0;
    healthCheckTimer = new QTimer(this);
    connect(healthCheckTimer, &QTimer::timeout, this, &MainWindow::checkBackendHealth);
    healthCheckTimer->start(1000);
}

void MainWindow::stopBackendServer() {
    if (healthCheckTimer) {
        healthCheckTimer->stop();
    }
    // Since the process is autonomous, we don't kill it here.
    // The user manages its lifecycle via the background console window.
}

void MainWindow::checkBackendHealth() {
    m_healthCheckAttempts++;

    // Use a dedicated one-shot network manager for the health check
    // to avoid conflicting with the main solve reply handler
    auto *healthMgr = new QNetworkAccessManager(this);
    QNetworkRequest req(QUrl("http://127.0.0.1:8080/health"));
    req.setTransferTimeout(2000);

    QNetworkReply *reply = healthMgr->get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply, healthMgr]() {
        bool alive = (reply->error() == QNetworkReply::NoError);
        reply->deleteLater();
        healthMgr->deleteLater();

        if (alive && !m_backendReady) {
            // Backend is now responsive!
            m_backendReady = true;
            healthCheckTimer->stop();
            solveButton->setEnabled(true);
            loginButton->setEnabled(true);
            backendStatusLabel->setText("✅ Backend Server Online");
            backendStatusLabel->setStyleSheet("color: #10b981; font-weight: bold; padding: 8px; border-radius: 6px; background-color: rgba(16,185,129,0.1); border: 1px solid rgba(16,185,129,0.3);");
            qDebug() << "[Backend] Server is READY after" << m_healthCheckAttempts << "attempts.";

            // Auto-hide the status label after 4 seconds
            QTimer::singleShot(4000, this, [this]() {
                backendStatusLabel->setVisible(false);
            });
        } else if (!alive && m_healthCheckAttempts >= 60) {
            // Timeout after ~30 seconds
            healthCheckTimer->stop();
            backendStatusLabel->setText("❌ Backend Failed to Start");
            backendStatusLabel->setStyleSheet("color: #ef4444; font-weight: bold; padding: 8px; border-radius: 6px; background-color: rgba(239,68,68,0.1); border: 1px solid rgba(239,68,68,0.3);");
            resultsLabel->setText("<b style='color:red;'>Backend Timeout:</b> The server did not respond within 60 seconds. "
                                  "If you are running the installed app, please reinstall to get the bundled backend. "
                                  "If developing, ensure .venv is set up and api_server.py has no import errors.");
            qDebug() << "[Backend] TIMEOUT - server did not become ready after 60s.";
        } else if (!alive) {
            // Still loading - update the visual indicator
            QString dots = QString(".").repeated((m_healthCheckAttempts % 3) + 1);
            backendStatusLabel->setText("⏳ Starting backend server" + dots);
        }
    });
}
