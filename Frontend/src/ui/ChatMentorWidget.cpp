#include "ChatMentorWidget.h"
#include <QScrollBar>

ChatMentorWidget::ChatMentorWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Chat Area
    chatScrollArea = new QScrollArea(this);
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setFrameShape(QFrame::NoFrame);
    chatScrollArea->setStyleSheet("QScrollArea { background-color: transparent; }");
    
    chatContainer = new QWidget();
    chatContainer->setStyleSheet("background-color: transparent;");
    chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setAlignment(Qt::AlignTop);
    chatLayout->setSpacing(8);
    chatScrollArea->setWidget(chatContainer);
    
    mainLayout->addWidget(chatScrollArea, 1);
    
    // Input Area
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputBox = new QLineEdit(this);
    inputBox->setPlaceholderText("Ask your mentor or answer the question...");
    inputBox->setMinimumHeight(40);
    
    sendButton = new QPushButton("Send", this);
    sendButton->setMinimumHeight(40);
    sendButton->setStyleSheet("background-color: #10b981; color: white; font-weight: bold; border-radius: 6px; padding: 0 15px;");
    
    inputLayout->addWidget(inputBox, 1);
    inputLayout->addWidget(sendButton);
    mainLayout->addLayout(inputLayout);

    connect(sendButton, &QPushButton::clicked, this, &ChatMentorWidget::onSendClicked);
    connect(inputBox, &QLineEdit::returnPressed, this, &ChatMentorWidget::onSendClicked);
    
    // Welcome message
    addMessage("Mentor", "Hello! I am your AI Turbomachinery Mentor. Let's solve some problems together. What would you like to work on?", true);
}

void ChatMentorWidget::onSendClicked() {
    QString text = inputBox->text().trimmed();
    if (text.isEmpty()) return;
    
    addMessage("You", text, false);
    inputBox->clear();
    
    // Show typing indicator
    addMessage("Mentor", "<i>Thinking...</i>", true);
    
    emit sendMessage(text);
}

void ChatMentorWidget::addMessage(const QString &sender, const QString &message, bool isMentor) {
    // Remove "Thinking..." message if it exists (it would be the last widget)
    if (!isMentor && chatLayout->count() > 0) {
        // Typically handled by whoever calls addMessage, but for simplicity we will just add.
        // In a full implementation, we'd find and remove the typing indicator.
    }

    QWidget *bubbleWidget = new QWidget();
    QVBoxLayout *bubbleLayout = new QVBoxLayout(bubbleWidget);
    bubbleLayout->setContentsMargins(12, 8, 12, 8);
    
    QLabel *senderLabel = new QLabel("<b>" + sender + "</b>");
    senderLabel->setStyleSheet("color: #94a3b8; font-size: 11px;");
    
    QLabel *msgLabel = new QLabel(message);
    msgLabel->setWordWrap(true);
    msgLabel->setTextFormat(Qt::RichText);
    
    bubbleLayout->addWidget(senderLabel);
    bubbleLayout->addWidget(msgLabel);
    
    if (isMentor) {
        bubbleWidget->setStyleSheet("QWidget { background-color: rgba(16, 185, 129, 0.15); border: 1px solid rgba(16, 185, 129, 0.3); border-radius: 8px; } QLabel { border: none; background: transparent; }");
    } else {
        bubbleWidget->setStyleSheet("QWidget { background-color: rgba(56, 189, 248, 0.15); border: 1px solid rgba(56, 189, 248, 0.3); border-radius: 8px; } QLabel { border: none; background: transparent; }");
    }
    
    chatLayout->addWidget(bubbleWidget);
    
    // Auto-scroll to bottom
    QTimer::singleShot(50, this, [this]() {
        chatScrollArea->verticalScrollBar()->setValue(chatScrollArea->verticalScrollBar()->maximum());
    });
}
