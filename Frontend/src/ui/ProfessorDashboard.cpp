#include "ProfessorDashboard.h"

ProfessorDashboard::ProfessorDashboard(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    
    QLabel *title = new QLabel("<b>Professor Dashboard</b>", this);
    title->setStyleSheet("font-size: 24px; color: #f8fafc;");
    layout->addWidget(title);
    
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->addWidget(createStatCard("Total Students", "42", "#3b82f6"));
    statsLayout->addWidget(createStatCard("Active Sessions", "12", "#10b981"));
    statsLayout->addWidget(createStatCard("Avg. Score", "85%", "#f59e0b"));
    
    layout->addLayout(statsLayout);
    
    QLabel *recentLabel = new QLabel("<b>Recent Topics Discussed</b><br>• Axial Compressors<br>• Euler Equation<br>• Velocity Triangles", this);
    recentLabel->setStyleSheet("font-size: 16px; color: #cbd5e1; margin-top: 20px;");
    layout->addWidget(recentLabel);
    
    layout->addStretch();
}

QWidget* ProfessorDashboard::createStatCard(const QString &title, const QString &value, const QString &color) {
    QFrame *card = new QFrame();
    card->setStyleSheet("QFrame { background-color: rgba(30, 41, 59, 0.8); border-radius: 10px; border: 1px solid rgba(255, 255, 255, 0.1); padding: 15px; }");
    
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: #94a3b8; font-size: 14px; font-weight: bold; border: none; background: transparent;");
    
    QLabel *valueLabel = new QLabel(value);
    valueLabel->setStyleSheet(QString("color: %1; font-size: 32px; font-weight: bold; border: none; background: transparent;").arg(color));
    
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(valueLabel);
    
    return card;
}
