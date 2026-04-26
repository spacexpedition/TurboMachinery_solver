#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QListWidget>

class SidebarWidget : public QWidget {
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget *parent = nullptr);
    void setDarkMode(bool isDark);

signals:
    void themeToggled();
    void modeSelected(int index); // To signal MainWindow to change the stacked widget

private slots:
    void onThemeToggleClicked();

private:
    QVBoxLayout *mainLayout;
    
    // Top section
    QLabel *appLogoLabel;
    QPushButton *newChatButton;
    QLineEdit *searchBox;
    
    // Middle section (Recent/Menu items)
    QListWidget *menuList;
    
    // Bottom section
    QPushButton *themeToggleBtn;
    QWidget *userProfileWidget;
    QLabel *userNameLabel;
    QLabel *userEmailLabel;

    bool m_isDarkMode = true; // Default to dark mode
    void applyTheme();
};

#endif // SIDEBARWIDGET_H
