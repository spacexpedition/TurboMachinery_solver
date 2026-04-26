#include "ChatMentorWidget.h"
#include <QScrollBar>
#include <QTimer>

ChatMentorWidget::ChatMentorWidget(QWidget *parent) : QWidget(parent) {
    // Main Chat Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // Remove outer margins to fit pane
    mainLayout->setSpacing(0);
    
    // Top Bar (Optional Title)
    QWidget *topBar = new QWidget();
    topBar->setMinimumHeight(60);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    QLabel *titleLabel = new QLabel("<b>Justlegal Chat</b>");
    titleLabel->setStyleSheet("font-size: 16px;");
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    mainLayout->addWidget(topBar);

    // Chat Area
    chatScrollArea = new QScrollArea(this);
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setFrameShape(QFrame::NoFrame);
    chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chatScrollArea->setStyleSheet("QScrollArea { background-color: transparent; border: none; } QScrollBar:vertical { width: 8px; background: transparent; } QScrollBar::handle:vertical { background: rgba(255, 255, 255, 0.2); border-radius: 4px; }");
    
    chatContainer = new QWidget();
    chatContainer->setStyleSheet("background-color: transparent;");
    chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setAlignment(Qt::AlignTop);
    chatLayout->setSpacing(20); // More spacing between bubbles
    chatLayout->setContentsMargins(40, 20, 40, 20); // Pad chat area
    chatScrollArea->setWidget(chatContainer);
    
    mainLayout->addWidget(chatScrollArea, 1);
    
    // Regenerate Button Area
    QHBoxLayout *regenLayout = new QHBoxLayout();
    regenLayout->setAlignment(Qt::AlignCenter);
    QPushButton *regenButton = new QPushButton("🔄 Regenerate");
    regenButton->setCursor(Qt::PointingHandCursor);
    regenButton->setStyleSheet("background-color: rgba(31, 41, 55, 0.5); border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 15px; padding: 6px 16px; color: #d1d5db; font-size: 12px; margin-bottom: 10px;");
    regenLayout->addWidget(regenButton);
    mainLayout->addLayout(regenLayout);

    // Input Area Container
    QWidget *inputContainer = new QWidget();
    QVBoxLayout *inputLayout = new QVBoxLayout(inputContainer);
    inputLayout->setContentsMargins(40, 10, 40, 20);
    
    // The actual text box (Pill shaped)
    QHBoxLayout *textInputLayout = new QHBoxLayout();
    inputBox = new QLineEdit(this);
    inputBox->setPlaceholderText("Ask Justlegal...");
    inputBox->setMinimumHeight(45);
    inputBox->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0.2); border: 1px solid rgba(255, 255, 255, 0.15); border-radius: 22px; padding: 0 20px; font-size: 14px; color: white; } QLineEdit::placeholder { color: #9ca3af; }");
    
    sendButton = new QPushButton("➤", this);
    sendButton->setCursor(Qt::PointingHandCursor);
    sendButton->setFixedSize(36, 36);
    sendButton->setStyleSheet("background-color: transparent; color: #9ca3af; font-size: 18px; border: none;");
    
    textInputLayout->addWidget(inputBox, 1);
    textInputLayout->addWidget(sendButton);
    
    inputLayout->addLayout(textInputLayout);

    // Bottom icons (Attachment, Voice)
    QHBoxLayout *bottomIconsLayout = new QHBoxLayout();
    QPushButton *attachBtn = new QPushButton("📎 Attachment");
    attachBtn->setStyleSheet("background: transparent; border: none; color: #6b7280; font-size: 12px;");
    QPushButton *voiceBtn = new QPushButton("🎤 Voice Message");
    voiceBtn->setStyleSheet("background: transparent; border: none; color: #6b7280; font-size: 12px;");
    
    bottomIconsLayout->addWidget(attachBtn);
    bottomIconsLayout->addWidget(voiceBtn);
    bottomIconsLayout->addStretch();
    
    QLabel *charCount = new QLabel("16/3000");
    charCount->setStyleSheet("color: #9ca3af; font-size: 11px;");
    bottomIconsLayout->addWidget(charCount);
    
    inputLayout->addLayout(bottomIconsLayout);
    mainLayout->addWidget(inputContainer);

    connect(sendButton, &QPushButton::clicked, this, &ChatMentorWidget::onSendClicked);
    connect(inputBox, &QLineEdit::returnPressed, this, &ChatMentorWidget::onSendClicked);
    
    // Welcome message
    addMessage("Mentor", "Absolutely! Let me gather some details about your requirements.", true);
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
    QHBoxLayout *rowLayout = new QHBoxLayout(bubbleWidget);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    
    // Avatar
    QLabel *avatarLabel = new QLabel();
    avatarLabel->setFixedSize(36, 36);
    avatarLabel->setStyleSheet(QString("border-radius: 18px; background-color: %1;").arg(isMentor ? "#f3e8ff" : "#e0f2fe"));
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setText(isMentor ? "🤖" : "👤");
    
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setSpacing(8);
    
    QLabel *msgLabel = new QLabel(message);
    msgLabel->setWordWrap(true);
    msgLabel->setTextFormat(Qt::RichText);
    msgLabel->setStyleSheet("font-size: 14px; line-height: 1.5;");
    
    contentLayout->addWidget(msgLabel);
    
    if (isMentor) {
        // Add action buttons for AI
        QHBoxLayout *actionsLayout = new QHBoxLayout();
        actionsLayout->setSpacing(15);
        actionsLayout->setAlignment(Qt::AlignLeft);
        
        QPushButton *upBtn = new QPushButton("👍");
        QPushButton *downBtn = new QPushButton("👎");
        QPushButton *copyBtn = new QPushButton("📋 Copy");
        QPushButton *shareBtn = new QPushButton("🔗 Share");
        
        QString actionStyle = "background: transparent; border: none; color: #9ca3af; font-size: 12px;";
        upBtn->setStyleSheet(actionStyle);
        downBtn->setStyleSheet(actionStyle);
        copyBtn->setStyleSheet(actionStyle);
        shareBtn->setStyleSheet(actionStyle);
        
        actionsLayout->addWidget(upBtn);
        actionsLayout->addWidget(downBtn);
        actionsLayout->addStretch();
        actionsLayout->addWidget(copyBtn);
        actionsLayout->addWidget(shareBtn);
        
        contentLayout->addLayout(actionsLayout);
    }
    
    if (isMentor) {
        rowLayout->addWidget(avatarLabel, 0, Qt::AlignTop);
        rowLayout->addLayout(contentLayout, 1);
        rowLayout->addStretch(0);
        msgLabel->setStyleSheet("font-size: 14px; line-height: 1.5; background-color: rgba(243, 232, 255, 0.6); padding: 12px 16px; border-radius: 12px; border-top-left-radius: 2px; border: 1px solid rgba(255, 255, 255, 0.3);");
    } else {
        rowLayout->addStretch(0);
        rowLayout->addLayout(contentLayout, 1);
        rowLayout->addWidget(avatarLabel, 0, Qt::AlignTop);
        msgLabel->setStyleSheet("font-size: 14px; line-height: 1.5; background-color: rgba(243, 244, 246, 0.6); padding: 12px 16px; border-radius: 12px; border-top-right-radius: 2px; border: 1px solid rgba(255, 255, 255, 0.3);");
    }
    
    chatLayout->addWidget(bubbleWidget);
    
    // Auto-scroll to bottom
    QTimer::singleShot(50, this, [this]() {
        chatScrollArea->verticalScrollBar()->setValue(chatScrollArea->verticalScrollBar()->maximum());
    });
}
