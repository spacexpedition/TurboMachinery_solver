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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Intelligent Turbomachinery Solver");
    setupUI();

    // Initialize the network manager for async HTTP calls
    networkManager = new QNetworkAccessManager(this);

    // Connect signals and slots
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::sendSolveRequest);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplyFinished);
}

MainWindow::~MainWindow() {
    // Qt's object tree handles deletion of parented widgets and networkManager
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    QLabel *titleLabel = new QLabel("<b>Enter Word Problem or Known Variables:</b>");
    titleLabel->setStyleSheet("font-size: 14px;");

    nlpInputArea = new QTextEdit();
    nlpInputArea->setPlaceholderText("e.g. An axial turbine stage has a blade speed of 100 m/s and constant meridional velocity of 50 m/s. The absolute inlet angle is 30 degrees...");
    nlpInputArea->setMaximumHeight(80);
    nlpInputArea->setStyleSheet("padding: 5px; font-size: 13px;");

    solveButton = new QPushButton("Solve Physics & Render Geometry");
    solveButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #0078D7; "
        "   color: white; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "   padding: 10px; "
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover { background-color: #005A9E; }"
        "QPushButton:disabled { background-color: #A0A0A0; }"
    );

    // Initialize our custom vector rendering widget
    canvasWidget = new VelocityTriangleWidget(this);

    resultsLabel = new QLabel("Results will appear here.");
    resultsLabel->setWordWrap(true);
    resultsLabel->setStyleSheet("background-color: #F0F0F0; padding: 10px; border-radius: 4px; font-family: monospace;");
    resultsLabel->setMinimumHeight(60);

    // Add widgets to layout
    layout->addWidget(titleLabel);
    layout->addWidget(nlpInputArea);
    layout->addWidget(solveButton);
    layout->addWidget(canvasWidget, 1); // The '1' gives the canvas stretch priority
    layout->addWidget(resultsLabel);

    setCentralWidget(centralWidget);
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