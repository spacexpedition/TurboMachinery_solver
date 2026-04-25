#ifndef PROFESSORDASHBOARD_H
#define PROFESSORDASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

class ProfessorDashboard : public QWidget {
    Q_OBJECT
public:
    explicit ProfessorDashboard(QWidget *parent = nullptr);

private:
    QWidget* createStatCard(const QString &title, const QString &value, const QString &color);
};

#endif // PROFESSORDASHBOARD_H
