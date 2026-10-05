#pragma once

#include <QString>
#include <QVector>
#include <QtGlobal>

struct Student
{
    QString name;
    int schoolYear = 1;
    double score = 70.0;
    int potential = 70;
    int diligence = 70;
    double stress = 30.0;
    double satisfaction = 70.0;
};

struct Teacher
{
    QString name;
    QString subject;
    int teaching = 70;
    int management = 70;
    qint64 monthlySalary = 3500000;
};

struct FinanceSnapshot
{
    qint64 revenue = 0;
    qint64 payroll = 0;
    qint64 rent = 0;
    qint64 net = 0;
};

class GameSimulation
{
public:
    GameSimulation();

    void advanceWeek();

    const QVector<Student>& students() const { return students_; }
    const QVector<Teacher>& teachers() const { return teachers_; }
    const FinanceSnapshot& lastFinance() const { return lastFinance_; }

    int week() const { return week_; }
    int reputation() const { return reputation_; }
    qint64 cash() const { return cash_; }

    double averageScore() const;
    double averageSatisfaction() const;
    QString dateText() const;

private:
    void createInitialData();
    void updateStudents();
    void updateFinance();
    void updateReputation();

    QVector<Student> students_;
    QVector<Teacher> teachers_;

    int week_ = 1;
    int startYear_ = 2027;
    int reputation_ = 42;
    qint64 cash_ = 50000000;
    FinanceSnapshot lastFinance_;
};
