#include <QCoreApplication>
#include <QNetworkAccessManager> // NOLINT
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>
#include <QDebug>
#include <QTimer>

/**
 * @class TurbomachineryClient
 * @brief Handles async JSON communication with the FastAPI Python backend.
 * * This class demonstrates the core networking logic used in the project,
 * sending a textbook scenario to the backend and parsing the solved results.
 */
class TurbomachineryClient : public QObject {
    Q_OBJECT
public:
    explicit TurbomachineryClient(QObject *parent = nullptr) : QObject(parent) {
        // Connect the direct member variable's signal to our slot
        connect(&manager, &QNetworkAccessManager::finished,
                this, &TurbomachineryClient::onReplyFinished);
    }

    /**
     * @brief Sends a POST request to the local physics solver API.
     */
    void sendSolveRequest() {
        const QUrl url("http://127.0.0.1:8080/api/solve");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // 1. Build the JSON Payload matching the Python Pydantic Schema
        QJsonObject knownVariables;
        knownVariables["Cm1"] = 50.0;
        knownVariables["alpha1"] = 0.5236; // 30 degrees in radians
        knownVariables["U1"] = 100.0;
        knownVariables["m_dot"] = 10.0;

        QJsonObject mainPayload;
        mainPayload["known_variables"] = knownVariables;

        const QJsonDocument doc(mainPayload);
        const QByteArray jsonData = doc.toJson();

        qDebug() << "Sending Request to Python Backend...";
        manager.post(request, jsonData);
    }

private slots:
    /**
     * @brief Handles the incoming response from the microservice.
     */
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void onReplyFinished(QNetworkReply *reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Network Error:" << reply->errorString();
        } else {
            // 3. Parse the JSON Response
            const QByteArray responseData = reply->readAll();
            const QJsonDocument doc = QJsonDocument::fromJson(responseData);
            const QJsonObject jsonObj = doc.object();

            qDebug() << "\n--- Received Response from Backend ---";
            qDebug() << "Status:" << jsonObj["status"].toString();
            qDebug() << "Message:" << jsonObj["message"].toString();

            const QJsonObject results = jsonObj["results"].toObject();
            qDebug() << "Solved State (Symbolic Graph Results):";
            for (auto it = results.begin(); it != results.end(); ++it) {
                qDebug().noquote() << "  " << it.key() << ":" << it.value().toDouble();
            }
        }

        reply->deleteLater();

        // Exit the test application event loop
        QCoreApplication::quit();
    }

private:
    // By using a direct member instead of a pointer (new),
    // we completely avoid Clang-Tidy memory leak warnings.
    QNetworkAccessManager manager;
};

// =====================================================================
// Entry point for testing the C++ Qt6 Network component independently.
// =====================================================================
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    TurbomachineryClient client;

    // Trigger the request once the event loop is active
    QTimer::singleShot(0, [&client]() {
        client.sendSolveRequest();
    });

    return +`-app.exec();
}

// CRITICAL: Since this file contains the definition of a Q_OBJECT class,
// we include the generated Meta-Object Compiler file here.
#include "main.moc"