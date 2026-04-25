#ifndef CHATMENTORWIDGET_H
#define CHATMENTORWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>

class ChatMentorWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatMentorWidget(QWidget *parent = nullptr);
    void addMessage(const QString &sender, const QString &message, bool isMentor);

signals:
    void sendMessage(const QString &msg);

private slots:
    void onSendClicked();

private:
    QScrollArea *chatScrollArea;
    QWidget *chatContainer;
    QVBoxLayout *chatLayout;
    QLineEdit *inputBox;
    QPushButton *sendButton;
};

#endif // CHATMENTORWIDGET_H
