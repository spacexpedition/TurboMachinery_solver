#include <QCoreApplication>
#include <QNetworkAccessManager>
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
 */
class TurbomachineryClient : public QObject {
    Q_OBJECT
public:
    explicit TurbomachineryClient(QObject *parent = nullptr) : QObject(parent) {
        manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished,
                this, &TurbomachineryClient::onReplyFinished);
    }

    void sendSolveRequest() {
        QUrl url("http://127.0.0.1:8080/api/solve");
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

        QJsonDocument doc(mainPayload);
        QByteArray jsonData = doc.toJson();

        qDebug() << "Sending Request to Backend:";
        qDebug().noquote() << jsonData;

        // 2. Fire the async POST request
        manager->post(request, jsonData);
    }

private slots:
    void onReplyFinished(QNetworkReply *reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Network Error:" << reply->errorString();
        } else {
            // 3. Parse the JSON Response
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObj = doc.object();

            qDebug() << "\n--- Received Response from Backend ---";
            qDebug() << "Status:" << jsonObj["status"].toString();
            qDebug() << "Message:" << jsonObj["message"].toString();

            QJsonObject results = jsonObj["results"].toObject();
            qDebug() << "Solved State:";
            for (auto it = results.begin(); it != results.end(); ++it) {
                qDebug().noquote() << "  " << it.key() << ":" << it.value().toDouble();
            }
        }

        reply->deleteLater();

        // Quit application after demonstrating the network call
        QCoreApplication::quit();
    }

private:
    QNetworkAccessManager *manager;
};

// =====================================================================
// Main function setup for testing the C++ Qt6 Network component
// =====================================================================
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    TurbomachineryClient client;

    // Use QTimer to ensure the event loop is running before firing the request
    QTimer::singleShot(0, [&client]() {
        client.sendSolveRequest();
    });

    return app.exec();
}

#include "QtNetworkClient.moc"