#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QtGlobal>

enum class TrainingPlan
{
    Balanced = 0,
    ScoreIntensive,
    Fundamentals,
    Care
};

struct Student
{
    QString name;
    QString subject;
    int schoolYear = 1;

    double score = 60.0;
    int potential = 70;
    int diligence = 60;
    int comprehension = 60;
    int memorization = 60;
    int concentration = 60;
    int examNerves = 60;

    double stress = 25.0;
    double fatigue = 20.0;
    double satisfaction = 65.0;
    double attendance = 90.0;

    qint64 monthlyTuition = 420000;

    QString conditionText() const;
};

struct Applicant
{
    Student student;
    int consultationFit = 70;
    int parentDemand = 50;
    QString note;
};

struct Teacher
{
    QString name;
    QString subject;

    int teaching = 70;
    int counseling = 60;
    int discipline = 60;
    int charisma = 60;
    int stamina = 70;

    qint64 monthlySalary = 3500000;
    qint64 signingCost = 1200000;

    QString trait;
};

struct FacilityState
{
    int classrooms = 1;
    int interiorLevel = 1;
    int studyRoomLevel = 0;

    int capacity() const { return classrooms * 18; }
};

struct FinanceSnapshot
{
    qint64 revenue = 0;
    qint64 payroll = 0;
    qint64 rent = 0;
    qint64 electricity = 0;
    qint64 water = 0;
    qint64 maintenance = 0;
    qint64 net = 0;
};

struct RivalAcademy
{
    QString name = "탑클래스 학원";
    QString specialty = "수학";
    int reputation = 48;
    int students = 42;
    qint64 monthlyTuition = 450000;
};

class GameSimulation
{
public:
    GameSimulation();

    void setupAcademy(const QString& academyName, const QString& specialty);
    void advanceWeek(TrainingPlan plan);

    bool acceptApplicant(int index);
    bool rejectApplicant(int index);
    bool hireTeacher(int index);

    bool upgradeInterior();
    bool expandClassroom();
    bool upgradeStudyRoom();

    const QVector<Student>& students() const { return students_; }
    const QVector<Applicant>& applicants() const { return applicants_; }
    const QVector<Teacher>& teachers() const { return teachers_; }
    const QVector<Teacher>& teacherCandidates() const { return teacherCandidates_; }

    const FinanceSnapshot& lastFinance() const { return lastFinance_; }
    const FacilityState& facilities() const { return facilities_; }
    const RivalAcademy& rival() const { return rival_; }

    const QString& academyName() const { return academyName_; }
    const QString& specialty() const { return specialty_; }
    const QString& latestEvent() const { return latestEvent_; }

    int week() const { return week_; }
    int reputation() const { return reputation_; }
    qint64 cash() const { return cash_; }

    double averageScore() const;
    double averageSatisfaction() const;
    double averageStress() const;

    QString dateText() const;
    QString trainingPlanName(TrainingPlan plan) const;

    qint64 interiorUpgradeCost() const;
    qint64 classroomExpansionCost() const;
    qint64 studyRoomUpgradeCost() const;

private:
    void generateApplicants(int targetCount = 6);
    void generateTeacherCandidates(int targetCount = 5);

    void updateStudents(TrainingPlan plan);
    void updateFinance();
    void updateReputation();
    void updateRival();
    void resolveWeeklyEvent();

    double teacherEffectFor(const QString& subject) const;
    int randomInt(int lowInclusive, int highInclusive) const;

    QString academyName_ = "새싹 학원";
    QString specialty_ = "수학";

    QVector<Student> students_;
    QVector<Applicant> applicants_;
    QVector<Teacher> teachers_;
    QVector<Teacher> teacherCandidates_;

    FacilityState facilities_;
    FinanceSnapshot lastFinance_;
    RivalAcademy rival_;

    int week_ = 1;
    int startYear_ = 2027;
    int reputation_ = 35;
    qint64 cash_ = 35000000;

    QString latestEvent_ = "개원을 준비하고 있습니다.";
};
