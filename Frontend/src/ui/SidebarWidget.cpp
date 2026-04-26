#include "SidebarWidget.h"
#include <QHBoxLayout>

SidebarWidget::SidebarWidget(QWidget *parent) : QWidget(parent) {
    setFixedWidth(280); // Fixed width for sidebar

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 20, 15, 20);
    mainLayout->setSpacing(15);

    // --- Top Section ---
    QHBoxLayout *topLayout = new QHBoxLayout();
    appLogoLabel = new QLabel("<b>Turbomachines</b> Solver");
    appLogoLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    
    newChatButton = new QPushButton("+ New Problem");
    newChatButton->setCursor(Qt::PointingHandCursor);
    newChatButton->setStyleSheet("background-color: #8B5CF6; color: white; border-radius: 6px; padding: 6px 12px; font-weight: bold;"); // Purple accent
    
    topLayout->addWidget(appLogoLabel);
    topLayout->addStretch();
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
    profileLayout->setContentsMargins(5, 5, 5, 5);
    
    QLabel *avatarLabel = new QLabel("👤");
    avatarLabel->setFixedSize(32, 32);
    avatarLabel->setStyleSheet("background-color: #e2e8f0; border-radius: 16px;");
    avatarLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *nameLayout = new QVBoxLayout();
    nameLayout->setSpacing(0);
    userNameLabel = new QLabel("<b>User</b>");
    userEmailLabel = new QLabel("user@example.com");
    userEmailLabel->setStyleSheet("color: #9ca3af; font-size: 11px;");
    nameLayout->addWidget(userNameLabel);
    nameLayout->addWidget(userEmailLabel);

    profileLayout->addWidget(avatarLabel);
    profileLayout->addLayout(nameLayout);
    profileLayout->addStretch();
    
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
        this->setStyleSheet("SidebarWidget { background-color: #1F2128; color: #FFFFFF; border-right: 1px solid #33333B; }");
        appLogoLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
        searchBox->setStyleSheet("background-color: #2D2F36; color: #FFFFFF; border-radius: 6px; padding: 5px; border: none;");
        themeToggleBtn->setText("☀️ Light");
        themeToggleBtn->setStyleSheet("text-align: left; padding: 8px; color: #9ca3af; background: transparent;");
        menuList->setStyleSheet("QListWidget { background: transparent; } QListWidget::item { color: #d1d5db; padding: 8px; border-radius: 6px; } QListWidget::item:selected { background-color: #2D2F36; color: #FFFFFF; font-weight: bold; } QListWidget::item:hover:!selected { background-color: #2D2F36; }");
        userProfileWidget->setStyleSheet("background-color: transparent; border-top: 1px solid #33333B;");
        userNameLabel->setStyleSheet("color: #FFFFFF;");
    } else {
        this->setStyleSheet("SidebarWidget { background-color: #F8F9FA; color: #111827; border-right: 1px solid #E5E7EB; }");
        appLogoLabel->setStyleSheet("color: #111827; font-size: 16px; font-weight: bold;");
        searchBox->setStyleSheet("background-color: #FFFFFF; color: #111827; border-radius: 6px; padding: 5px; border: 1px solid #E5E7EB;");
        themeToggleBtn->setText("🌙 Dark");
        themeToggleBtn->setStyleSheet("text-align: left; padding: 8px; color: #6b7280; background: transparent;");
        menuList->setStyleSheet("QListWidget { background: transparent; } QListWidget::item { color: #4b5563; padding: 8px; border-radius: 6px; } QListWidget::item:selected { background-color: #FFFFFF; color: #8B5CF6; font-weight: bold; border: 1px solid #E5E7EB; } QListWidget::item:hover:!selected { background-color: #E5E7EB; }");
        userProfileWidget->setStyleSheet("background-color: transparent; border-top: 1px solid #E5E7EB;");
        userNameLabel->setStyleSheet("color: #111827;");
    }
}
