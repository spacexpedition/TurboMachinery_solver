#include "SidebarWidget.h"
#include <QHBoxLayout>

SidebarWidget::SidebarWidget(QWidget *parent) : QWidget(parent) {
    setFixedWidth(280); // Fixed width for sidebar

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 20, 15, 20);
    mainLayout->setSpacing(15);

    // --- Top Section ---
    QVBoxLayout *topLayout = new QVBoxLayout();
    
    appLogoLabel = new QLabel("<b>Turbomachines</b> Solver");
    appLogoLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    topLayout->addWidget(appLogoLabel);
    
    newChatButton = new QPushButton("+ New Problem");
    newChatButton->setCursor(Qt::PointingHandCursor);
    newChatButton->setStyleSheet("background-color: #8B5CF6; color: white; border-radius: 6px; padding: 10px; font-weight: bold; margin-top: 10px;"); // Purple accent, full width style
    topLayout->addWidget(newChatButton);
    
    mainLayout->addLayout(topLayout);

    // --- Search ---
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("🔍 Search...");
    searchBox->setMinimumHeight(35);
    mainLayout->addWidget(searchBox);

    // --- Menu / Recent List ---
    QLabel *recentLabel = new QLabel("<b>Recent</b>");
    recentLabel->setStyleSheet("color: #9ca3af; font-size: 12px; margin-top: 10px;");
    mainLayout->addWidget(recentLabel);

    menuList = new QListWidget();
    menuList->setFrameShape(QFrame::NoFrame);
    menuList->setStyleSheet("QListWidget { background: transparent; } QListWidget::item { padding: 8px; border-radius: 6px; }");
    
    // Default items
    QListWidgetItem *mentorItem = new QListWidgetItem("🤖 AI Mentor Chat");
    QListWidgetItem *solverItem = new QListWidgetItem("📐 Physics Solver");
    QListWidgetItem *dashboardItem = new QListWidgetItem("📊 Professor Dashboard");
    
    menuList->addItem(mentorItem);
    menuList->addItem(solverItem);
    menuList->addItem(dashboardItem);
    menuList->setCurrentRow(0); // Default to chat
    
    connect(menuList, &QListWidget::currentRowChanged, this, [this](int currentRow) {
        emit modeSelected(currentRow);
    });

    mainLayout->addWidget(menuList, 1); // Stretches to fill middle space

    // --- Bottom Section ---
    themeToggleBtn = new QPushButton();
    themeToggleBtn->setCursor(Qt::PointingHandCursor);
    themeToggleBtn->setFlat(true);
    connect(themeToggleBtn, &QPushButton::clicked, this, &SidebarWidget::onThemeToggleClicked);
    mainLayout->addWidget(themeToggleBtn);

    // User Profile
    userProfileWidget = new QWidget();
    QHBoxLayout *profileLayout = new QHBoxLayout(userProfileWidget);
    profileLayout->setContentsMargins(5, 10, 5, 5); // Add top margin
    profileLayout->setSpacing(12); // Add spacing between avatar and text
    
    QLabel *avatarLabel = new QLabel("👤");
    avatarLabel->setFixedSize(40, 40); // Make avatar bigger
    avatarLabel->setStyleSheet("background-color: #e2e8f0; border-radius: 20px; font-size: 20px;");
    avatarLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *nameLayout = new QVBoxLayout();
    nameLayout->setSpacing(2); // Slight spacing between name and email
    userNameLabel = new QLabel("<b>User</b>");
    userEmailLabel = new QLabel("user@example.com");
    userEmailLabel->setStyleSheet("color: #9ca3af; font-size: 11px;");
    nameLayout->addWidget(userNameLabel);
    nameLayout->addWidget(userEmailLabel);
    nameLayout->addStretch(); // Push text to top

    profileLayout->addWidget(avatarLabel);
    profileLayout->addLayout(nameLayout, 1); // Allow name layout to stretch
    
    mainLayout->addWidget(userProfileWidget);

    applyTheme(); // Initial theme setup
}

void SidebarWidget::onThemeToggleClicked() {
    m_isDarkMode = !m_isDarkMode;
    applyTheme();
    emit themeToggled(); // Notify main window to update other components
}

void SidebarWidget::setDarkMode(bool isDark) {
    if (m_isDarkMode != isDark) {
        m_isDarkMode = isDark;
        applyTheme();
    }
}

void SidebarWidget::applyTheme() {
    if (m_isDarkMode) {
        this->setStyleSheet("SidebarWidget { background-color: rgba(31, 33, 40, 0.4); color: #FFFFFF; border-right: 1px solid rgba(255, 255, 255, 0.05); }");
        appLogoLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold; margin-bottom: 5px;");
        searchBox->setStyleSheet("QLineEdit { background-color: rgba(45, 47, 54, 0.5); color: #FFFFFF; border-radius: 17px; padding: 5px 15px; border: 1px solid rgba(255, 255, 255, 0.1); } QLineEdit::placeholder { color: #9ca3af; }");
        themeToggleBtn->setText("☀️ Light");
        themeToggleBtn->setStyleSheet("text-align: left; padding: 8px; color: #9ca3af; background: transparent; font-weight: 500;");
        menuList->setStyleSheet("QListWidget { background: transparent; outline: 0; } QListWidget::item { color: #d1d5db; padding: 10px; border-radius: 8px; margin-bottom: 2px; } QListWidget::item:selected { background-color: rgba(139, 92, 246, 0.15); color: #FFFFFF; font-weight: bold; border-left: 3px solid #8B5CF6; border-top-left-radius: 0; border-bottom-left-radius: 0; } QListWidget::item:hover:!selected { background-color: rgba(255, 255, 255, 0.05); }");
        userProfileWidget->setStyleSheet("background-color: transparent; border-top: 1px solid rgba(255, 255, 255, 0.05); padding-top: 10px; margin-top: 10px;");
        userNameLabel->setStyleSheet("color: #FFFFFF; font-size: 13px;");
    } else {
        this->setStyleSheet("SidebarWidget { background-color: rgba(248, 249, 250, 0.6); color: #111827; border-right: 1px solid rgba(229, 231, 235, 0.5); }");
        appLogoLabel->setStyleSheet("color: #111827; font-size: 16px; font-weight: bold; margin-bottom: 5px;");
        searchBox->setStyleSheet("QLineEdit { background-color: rgba(255, 255, 255, 0.7); color: #111827; border-radius: 17px; padding: 5px 15px; border: 1px solid rgba(229, 231, 235, 0.8); } QLineEdit::placeholder { color: #6b7280; }");
        themeToggleBtn->setText("🌙 Dark");
        themeToggleBtn->setStyleSheet("text-align: left; padding: 8px; color: #6b7280; background: transparent; font-weight: 500;");
        menuList->setStyleSheet("QListWidget { background: transparent; outline: 0; } QListWidget::item { color: #4b5563; padding: 10px; border-radius: 8px; margin-bottom: 2px; } QListWidget::item:selected { background-color: rgba(139, 92, 246, 0.1); color: #8B5CF6; font-weight: bold; border-left: 3px solid #8B5CF6; border-top-left-radius: 0; border-bottom-left-radius: 0; } QListWidget::item:hover:!selected { background-color: rgba(0, 0, 0, 0.03); }");
        userProfileWidget->setStyleSheet("background-color: transparent; border-top: 1px solid rgba(229, 231, 235, 0.5); padding-top: 10px; margin-top: 10px;");
        userNameLabel->setStyleSheet("color: #111827; font-size: 13px;");
    }
}
