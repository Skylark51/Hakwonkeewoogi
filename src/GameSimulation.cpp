#include "GameSimulation.h"

#include <QRandomGenerator>
#include <QtGlobal>

#include <numeric>

namespace
{
double clampValue(double value, double low, double high)
{
    return qBound(low, value, high);
}
}

GameSimulation::GameSimulation()
{
    createInitialData();
    updateFinance();
}

void GameSimulation::createInitialData()
{
    teachers_ = {
        {"김도현", "수학", 84, 73, 4300000},
        {"박서윤", "영어", 78, 88, 3900000},
        {"이준호", "국어", 74, 82, 3600000}
    };

    const QVector<QString> surnames = {"김", "이", "박", "최", "정", "강", "조", "윤"};
    const QVector<QString> givenNames = {
        "민준", "서준", "도윤", "지호", "예준", "서연", "지우", "하윤",
        "민서", "지민", "수빈", "현우", "유진", "채원", "시우", "예은"
    };

    auto* rng = QRandomGenerator::global();

    for (int i = 0; i < 36; ++i)
    {
        Student student;
        student.name = surnames.at(rng->bounded(surnames.size()))
                     + givenNames.at(rng->bounded(givenNames.size()));
        student.schoolYear = 1 + rng->bounded(3);
        student.score = 55.0 + rng->bounded(36);
        student.potential = 55 + rng->bounded(41);
        student.diligence = 40 + rng->bounded(61);
        student.stress = 15.0 + rng->bounded(45);
        student.satisfaction = 55.0 + rng->bounded(36);
        students_.push_back(student);
    }
}

void GameSimulation::advanceWeek()
{
    updateStudents();
    updateFinance();
    updateReputation();
    ++week_;
}

void GameSimulation::updateStudents()
{
    if (teachers_.isEmpty())
        return;

    double averageTeaching = 0.0;
    double averageManagement = 0.0;

    for (const auto& teacher : teachers_)
    {
        averageTeaching += teacher.teaching;
        averageManagement += teacher.management;
    }

    averageTeaching /= teachers_.size();
    averageManagement /= teachers_.size();

    auto* rng = QRandomGenerator::global();

    for (auto& student : students_)
    {
        const double randomFactor = (rng->bounded(9) - 4) * 0.08;

        const double growth =
            0.20
            + averageTeaching * 0.010
            + student.diligence * 0.006
            + student.potential * 0.003
            - student.stress * 0.004
            + randomFactor;

        student.score = clampValue(student.score + growth, 0.0, 100.0);

        const double stressDelta =
            2.3
            - averageManagement * 0.018
            + (rng->bounded(7) - 3) * 0.25;

        student.stress = clampValue(student.stress + stressDelta, 0.0, 100.0);

        const double satisfactionDelta =
            (growth * 0.40)
            + averageManagement * 0.006
            - student.stress * 0.008
            + (rng->bounded(5) - 2) * 0.20;

        student.satisfaction =
            clampValue(student.satisfaction + satisfactionDelta, 0.0, 100.0);
    }
}

void GameSimulation::updateFinance()
{
    constexpr qint64 monthlyTuitionPerStudent = 420000;
    constexpr qint64 monthlyRent = 6500000;
    constexpr double weeksPerMonth = 4.33;

    const qint64 monthlyRevenue =
        static_cast<qint64>(students_.size()) * monthlyTuitionPerStudent;

    qint64 monthlyPayroll = 0;
    for (const auto& teacher : teachers_)
        monthlyPayroll += teacher.monthlySalary;

    lastFinance_.revenue =
        static_cast<qint64>(monthlyRevenue / weeksPerMonth);
    lastFinance_.payroll =
        static_cast<qint64>(monthlyPayroll / weeksPerMonth);
    lastFinance_.rent =
        static_cast<qint64>(monthlyRent / weeksPerMonth);

    lastFinance_.net =
        lastFinance_.revenue - lastFinance_.payroll - lastFinance_.rent;

    cash_ += lastFinance_.net;
}

void GameSimulation::updateReputation()
{
    const double scoreComponent = averageScore() * 0.55;
    const double satisfactionComponent = averageSatisfaction() * 0.35;
    const double scaleComponent = qMin(10.0, students_.size() / 10.0);

    reputation_ = qBound(
        0,
        static_cast<int>(scoreComponent + satisfactionComponent + scaleComponent),
        100
    );
}

double GameSimulation::averageScore() const
{
    if (students_.isEmpty())
        return 0.0;

    double sum = 0.0;
    for (const auto& student : students_)
        sum += student.score;

    return sum / students_.size();
}

double GameSimulation::averageSatisfaction() const
{
    if (students_.isEmpty())
        return 0.0;

    double sum = 0.0;
    for (const auto& student : students_)
        sum += student.satisfaction;

    return sum / students_.size();
}

QString GameSimulation::dateText() const
{
    const int zeroBasedMonthOffset = (week_ - 1) / 4;
    int month = 3 + zeroBasedMonthOffset;
    int year = startYear_;

    while (month > 12)
    {
        month -= 12;
        ++year;
    }

    const int weekOfMonth = ((week_ - 1) % 4) + 1;

    return QString("%1년 %2월 %3주차")
        .arg(year)
        .arg(month)
        .arg(weekOfMonth);
}
