#pragma once

#include "GameSimulation.h"

#include <QMainWindow>

class QComboBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QStackedWidget;
class QTableWidget;
class QTableWidgetItem;
class QTextBrowser;
class QWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QWidget* createIntroPage();
    QWidget* createSetupPage();
    QWidget* createGamePage();

    QWidget* createDashboardTab();
    QWidget* createConsultationTab();
    QWidget* createStudentsTab();
    QWidget* createTeachersTab();
    QWidget* createTrainingTab();
    QWidget* createFacilitiesTab();
    QWidget* createFinanceTab();
    QWidget* createRivalTab();

    QWidget* createMetricCard(const QString& title, QLabel*& valueLabel);

    void editIntro();
    void beginSetup();
    void startGame();

    void acceptSelectedApplicant();
    void rejectSelectedApplicant();
    void hireSelectedTeacher();

    void advanceWeeks(int count);
    TrainingPlan selectedTrainingPlan() const;

    void refreshAll();
    void refreshApplicants();
    void refreshStudents();
    void refreshTeachers();
    void refreshFacilities();
    void refreshFinance();
    void refreshRival();
    void refreshTrainingDescription();

    QString defaultIntroText() const;
    static QString money(qint64 value);
    static QTableWidgetItem* centeredItem(const QString& text);

    GameSimulation simulation_;

    QStackedWidget* rootStack_ = nullptr;
    QWidget* introPage_ = nullptr;
    QWidget* setupPage_ = nullptr;
    QWidget* gamePage_ = nullptr;

    QTextBrowser* introBrowser_ = nullptr;
    QLineEdit* academyNameEdit_ = nullptr;
    QComboBox* specialtyCombo_ = nullptr;

    QLabel* academyTitleLabel_ = nullptr;
    QLabel* dateLabel_ = nullptr;
    QLabel* eventLabel_ = nullptr;

    QLabel* cashLabel_ = nullptr;
    QLabel* studentCountLabel_ = nullptr;
    QLabel* reputationLabel_ = nullptr;
    QLabel* averageScoreLabel_ = nullptr;
    QLabel* averageSatisfactionLabel_ = nullptr;
    QLabel* averageStressLabel_ = nullptr;
    QLabel* capacityLabel_ = nullptr;
    QLabel* ownerStatusLabel_ = nullptr;
    QLabel* weeklyNetLabel_ = nullptr;

    QProgressBar* reputationBar_ = nullptr;
    QProgressBar* scoreBar_ = nullptr;
    QProgressBar* satisfactionBar_ = nullptr;
    QProgressBar* stressBar_ = nullptr;

    QTableWidget* applicantTable_ = nullptr;
    QTableWidget* studentTable_ = nullptr;
    QTableWidget* teacherTable_ = nullptr;
    QTableWidget* teacherCandidateTable_ = nullptr;

    QComboBox* trainingCombo_ = nullptr;
    QLabel* trainingDescriptionLabel_ = nullptr;

    QLabel* classroomLabel_ = nullptr;
    QLabel* interiorLabel_ = nullptr;
    QLabel* studyRoomLabel_ = nullptr;
    QLabel* facilityCapacityLabel_ = nullptr;
    QLabel* interiorCostLabel_ = nullptr;
    QLabel* classroomCostLabel_ = nullptr;
    QLabel* studyRoomCostLabel_ = nullptr;

    QLabel* revenueLabel_ = nullptr;
    QLabel* payrollLabel_ = nullptr;
    QLabel* rentLabel_ = nullptr;
    QLabel* electricityLabel_ = nullptr;
    QLabel* waterLabel_ = nullptr;
    QLabel* maintenanceLabel_ = nullptr;
    QLabel* financeNetLabel_ = nullptr;

    QLabel* rivalNameLabel_ = nullptr;
    QLabel* rivalSpecialtyLabel_ = nullptr;
    QLabel* rivalReputationLabel_ = nullptr;
    QLabel* rivalStudentsLabel_ = nullptr;
    QLabel* rivalrySummaryLabel_ = nullptr;
};
