#pragma once

#include "GameSimulation.h"

#include <QMainWindow>

class QLabel;
class QProgressBar;
class QTableWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QWidget* createDashboardTab();
    QWidget* createStudentsTab();
    QWidget* createTeachersTab();
    QWidget* createFinanceTab();
    QWidget* createMetricCard(const QString& title, QLabel*& valueLabel);

    void refreshAll();
    void refreshStudents();
    void refreshTeachers();
    void refreshFinance();
    void advanceWeeks(int count);

    static QString money(qint64 value);

    GameSimulation simulation_;

    QLabel* dateLabel_ = nullptr;
    QLabel* cashLabel_ = nullptr;
    QLabel* studentCountLabel_ = nullptr;
    QLabel* reputationLabel_ = nullptr;
    QLabel* averageScoreLabel_ = nullptr;
    QLabel* averageSatisfactionLabel_ = nullptr;
    QLabel* weeklyNetLabel_ = nullptr;

    QProgressBar* reputationBar_ = nullptr;
    QProgressBar* scoreBar_ = nullptr;
    QProgressBar* satisfactionBar_ = nullptr;

    QTableWidget* studentTable_ = nullptr;
    QTableWidget* teacherTable_ = nullptr;

    QLabel* revenueLabel_ = nullptr;
    QLabel* payrollLabel_ = nullptr;
    QLabel* rentLabel_ = nullptr;
};
