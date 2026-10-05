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

    // score = 평소 학업능력. 실제 시험 점수는 lastExamScore에 별도로 기록한다.
    double score = 60.0;
    double lastExamScore = 0.0;
    int examCount = 0;

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

    qint64 monthlyTuition = 400000;

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

struct SubjectOffer
{
    QString subject;
    qint64 standardFee = 300000;
    qint64 currentFee = 300000;
    qint64 minFee = 180000;
    qint64 maxFee = 360000;
    bool opened = false;
};

struct FacilityState
{
    int classrooms = 1;
    int interiorLevel = 1;
    int floorExpansionLevel = 0;

    bool directorOffice = false;
    bool consultationRoom = false;
    bool pantry = false;
    int studyRooms = 0;

    int capacity() const { return classrooms * 18; }
    int roomSlots() const { return 3 + floorExpansionLevel * 2; }
    int usedRoomSlots() const
    {
        return classrooms
            + (directorOffice ? 1 : 0)
            + (consultationRoom ? 1 : 0)
            + (pantry ? 1 : 0)
            + studyRooms;
    }
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

struct WeekSummary
{
    qint64 cashBefore = 0;
    qint64 cashAfter = 0;
    int reputationBefore = 0;
    int reputationAfter = 0;
    double skillBefore = 0.0;
    double skillAfter = 0.0;
    double satisfactionBefore = 0.0;
    double satisfactionAfter = 0.0;

    int newApplicants = 0;
    int dropouts = 0;

    bool examOccurred = false;
    double examAverage = 0.0;
    QString examName;

    QString eventText;
    QString tip;
};

class GameSimulation
{
public:
    GameSimulation();

    void setupAcademy(const QString& academyName, const QString& specialty);
    WeekSummary advanceWeek(TrainingPlan plan);

    bool acceptApplicant(int index);
    bool rejectApplicant(int index);
    bool hireTeacher(int index);

    bool setTuition(int subjectIndex, qint64 monthlyFee);
    bool openSubject(int subjectIndex);
    bool closeSubject(int subjectIndex);

    bool upgradeInterior();
    bool expandFloor();
    bool addClassroom();
    bool buildDirectorOffice();
    bool buildConsultationRoom();
    bool buildPantry();
    bool buildStudyRoom();

    const QVector<Student>& students() const { return students_; }
    const QVector<Applicant>& applicants() const { return applicants_; }
    const QVector<Teacher>& teachers() const { return teachers_; }
    const QVector<Teacher>& teacherCandidates() const { return teacherCandidates_; }
    const QVector<SubjectOffer>& subjectOffers() const { return subjectOffers_; }

    const FinanceSnapshot& lastFinance() const { return lastFinance_; }
    const FacilityState& facilities() const { return facilities_; }
    const RivalAcademy& rival() const { return rival_; }
    const WeekSummary& lastWeekSummary() const { return lastWeekSummary_; }

    const QString& academyName() const { return academyName_; }
    const QString& specialty() const { return specialty_; }
    const QString& latestEvent() const { return latestEvent_; }

    int week() const { return week_; }
    int reputation() const { return reputation_; }
    qint64 cash() const { return cash_; }

    double averageScore() const;
    double averageExamScore() const;
    double averageSatisfaction() const;
    double averageStress() const;
    double averageTuitionRatio() const;

    QString dateText() const;
    QString nextAssessmentText() const;
    QString trainingPlanName(TrainingPlan plan) const;
    QString weeklyTip() const;

    bool hasTeacherForSubject(const QString& subject) const;
    bool canOpenSubject(int subjectIndex) const;

    qint64 interiorUpgradeCost() const;
    qint64 floorExpansionCost() const;
    qint64 classroomCost() const;
    qint64 directorOfficeCost() const;
    qint64 consultationRoomCost() const;
    qint64 pantryCost() const;
    qint64 studyRoomCost() const;

private:
    void initializeSubjectOffers();
    void generateApplicants(int count, bool guaranteeHighPotential = false);
    int generateWeeklyApplicants();
    void generateTeacherCandidates(int targetCount = 5);

    void updateStudents(TrainingPlan plan);
    int processDropouts();
    void updateFinance();
    void updateReputation();
    void updateRival();
    void resolveWeeklyEvent();

    bool runScheduledExam(WeekSummary& summary);

    double teacherEffectFor(const QString& subject) const;
    int randomInt(int lowInclusive, int highInclusive) const;

    int currentMonth() const;
    int currentWeekOfMonth() const;
    qint64 tuitionForSubject(const QString& subject) const;
    double tuitionRatioForSubject(const QString& subject) const;
    int randomPotential(bool forceHigh = false) const;
    QString randomOpenedSubject() const;
    bool hasFreeRoomSlot() const;

    QString academyName_ = "새싹 학원";
    QString specialty_ = "수학";

    QVector<Student> students_;
    QVector<Applicant> applicants_;
    QVector<Teacher> teachers_;
    QVector<Teacher> teacherCandidates_;
    QVector<SubjectOffer> subjectOffers_;

    FacilityState facilities_;
    FinanceSnapshot lastFinance_;
    RivalAcademy rival_;
    WeekSummary lastWeekSummary_;

    int week_ = 1;
    int startYear_ = 2027;
    int reputation_ = 35;
    qint64 cash_ = 35000000;

    double lastExamAverage_ = 0.0;
    QString latestEvent_ = "개원을 준비하고 있습니다.";
};
