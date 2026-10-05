#include "MainWindow.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("학원키우기 v0.2");
    resize(1380, 860);
    setMinimumSize(1120, 720);

    rootStack_ = new QStackedWidget(this);

    introPage_ = createIntroPage();
    setupPage_ = createSetupPage();
    gamePage_ = createGamePage();

    rootStack_->addWidget(introPage_);
    rootStack_->addWidget(setupPage_);
    rootStack_->addWidget(gamePage_);

    setCentralWidget(rootStack_);
    rootStack_->setCurrentWidget(introPage_);

    setStyleSheet(R"(
        QMainWindow, QWidget {
            background: #f3f5f7;
            color: #20242a;
            font-size: 14px;
        }

        QLabel#gameTitle {
            color: #152238;
            font-size: 24px;
            font-weight: 800;
        }

        QLabel#pageTitle {
            color: #172b4d;
            font-size: 30px;
            font-weight: 900;
        }

        QLabel#subtitle {
            color: #657080;
            font-size: 14px;
        }

        QLabel#sectionTitle {
            color: #172b4d;
            font-size: 17px;
            font-weight: 800;
        }

        QLabel#metricTitle {
            color: #66717f;
            font-size: 12px;
        }

        QLabel#metricValue {
            color: #172b4d;
            font-size: 22px;
            font-weight: 800;
        }

        QLabel#eventBox {
            background: #fff8df;
            border: 1px solid #e4d49b;
            border-radius: 8px;
            padding: 12px;
            color: #4b4120;
        }

        QFrame#card, QFrame#metricCard {
            background: white;
            border: 1px solid #dce1e7;
            border-radius: 10px;
        }

        QPushButton {
            background: #253858;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 9px 16px;
            font-weight: 700;
        }

        QPushButton:hover {
            background: #334e78;
        }

        QPushButton#secondaryButton {
            background: #e7ebf0;
            color: #26374f;
            border: 1px solid #d0d6de;
        }

        QPushButton#dangerButton {
            background: #8f3b3b;
        }

        QPushButton#facilityButton {
            background: #52667f;
        }

        QLineEdit, QComboBox, QTextEdit, QTextBrowser {
            background: white;
            border: 1px solid #ccd3dc;
            border-radius: 6px;
            padding: 8px;
        }

        QTextBrowser#introBox {
            font-size: 15px;
            line-height: 1.6;
            padding: 20px;
        }

        QTabWidget::pane {
            border: 1px solid #d9dee5;
            background: white;
            border-radius: 8px;
        }

        QTabBar::tab {
            background: #e8ecf1;
            border: none;
            padding: 11px 17px;
            margin-right: 2px;
        }

        QTabBar::tab:selected {
            background: #ffffff;
            font-weight: 800;
        }

        QTableWidget {
            background: white;
            alternate-background-color: #f8fafc;
            gridline-color: #e5e9ef;
            border: 1px solid #dde2e8;
        }

        QHeaderView::section {
            background: #edf1f5;
            color: #344050;
            border: none;
            border-bottom: 1px solid #d9dee5;
            padding: 8px;
            font-weight: 700;
        }

        QProgressBar {
            min-height: 18px;
            border: 1px solid #d7dde5;
            border-radius: 6px;
            background: #eef1f5;
            text-align: center;
        }

        QProgressBar::chunk {
            background: #5c7cfa;
            border-radius: 5px;
        }
    )");
}

QWidget* MainWindow::createIntroPage()
{
    auto* page = new QWidget();
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(110, 65, 110, 65);
    root->setSpacing(18);

    auto* title = new QLabel("학원키우기");
    title->setObjectName("pageTitle");

    auto* subtitle = new QLabel(
        "작은 학원 하나에서 시작해 학생, 강사, 시설, 재정과 경쟁을 관리하는 경영 시뮬레이션"
    );
    subtitle->setObjectName("subtitle");

    introBrowser_ = new QTextBrowser();
    introBrowser_->setObjectName("introBox");
    introBrowser_->setMinimumHeight(380);

    QSettings settings("Skylark51", "HakwonKeewoogi");
    introBrowser_->setPlainText(
        settings.value("intro/text", defaultIntroText()).toString()
    );

    auto* buttonRow = new QHBoxLayout();

    auto* editButton = new QPushButton("인트로 직접 편집");
    editButton->setObjectName("secondaryButton");

    auto* newGameButton = new QPushButton("새 게임 시작");
    newGameButton->setMinimumWidth(160);

    connect(editButton, &QPushButton::clicked, this, [this] {
        editIntro();
    });

    connect(newGameButton, &QPushButton::clicked, this, [this] {
        beginSetup();
    });

    buttonRow->addWidget(editButton);
    buttonRow->addStretch();
    buttonRow->addWidget(newGameButton);

    root->addWidget(title);
    root->addWidget(subtitle);
    root->addSpacing(8);
    root->addWidget(introBrowser_, 1);
    root->addLayout(buttonRow);

    return page;
}

QWidget* MainWindow::createSetupPage()
{
    auto* page = new QWidget();
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(180, 90, 180, 90);
    root->setSpacing(18);

    auto* title = new QLabel("학원 개원");
    title->setObjectName("pageTitle");

    auto* subtitle = new QLabel(
        "처음에는 원장 혼자 시작합니다. 주력 과목은 초기 상담 유입과 원장 직강 효율에 영향을 줍니다."
    );
    subtitle->setObjectName("subtitle");
    subtitle->setWordWrap(true);

    auto* card = new QFrame();
    card->setObjectName("card");
    auto* form = new QGridLayout(card);
    form->setContentsMargins(28, 28, 28, 28);
    form->setHorizontalSpacing(22);
    form->setVerticalSpacing(18);

    academyNameEdit_ = new QLineEdit("새싹 학원");
    specialtyCombo_ = new QComboBox();
    specialtyCombo_->addItems({"수학", "영어", "국어", "과학"});

    form->addWidget(new QLabel("학원 이름"), 0, 0);
    form->addWidget(academyNameEdit_, 0, 1);
    form->addWidget(new QLabel("주력 과목"), 1, 0);
    form->addWidget(specialtyCombo_, 1, 1);

    auto* guide = new QLabel(
        "시작 자금 3,500만원 · 강의실 1개 · 정원 18명 · 강사 0명\n"
        "주력 과목은 원장이 직접 가르치며, 개원 직후 상담 학생과 강사 후보가 생성됩니다."
    );
    guide->setWordWrap(true);
    guide->setObjectName("subtitle");
    form->addWidget(guide, 2, 0, 1, 2);

    auto* buttons = new QHBoxLayout();

    auto* backButton = new QPushButton("인트로로");
    backButton->setObjectName("secondaryButton");

    auto* startButton = new QPushButton("개원하기");

    connect(backButton, &QPushButton::clicked, this, [this] {
        rootStack_->setCurrentWidget(introPage_);
    });

    connect(startButton, &QPushButton::clicked, this, [this] {
        startGame();
    });

    buttons->addWidget(backButton);
    buttons->addStretch();
    buttons->addWidget(startButton);

    root->addWidget(title);
    root->addWidget(subtitle);
    root->addSpacing(12);
    root->addWidget(card);
    root->addStretch();
    root->addLayout(buttons);

    return page;
}

QWidget* MainWindow::createGamePage()
{
    auto* page = new QWidget();
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(18, 16, 18, 18);
    root->setSpacing(12);

    auto* topBar = new QHBoxLayout();

    academyTitleLabel_ = new QLabel("학원키우기");
    academyTitleLabel_->setObjectName("gameTitle");

    dateLabel_ = new QLabel();

    auto* weekButton = new QPushButton("1주 진행");
    auto* monthButton = new QPushButton("4주 진행");

    connect(weekButton, &QPushButton::clicked, this, [this] {
        advanceWeeks(1);
    });

    connect(monthButton, &QPushButton::clicked, this, [this] {
        advanceWeeks(4);
    });

    topBar->addWidget(academyTitleLabel_);
    topBar->addSpacing(18);
    topBar->addWidget(dateLabel_);
    topBar->addStretch();
    topBar->addWidget(weekButton);
    topBar->addWidget(monthButton);

    eventLabel_ = new QLabel();
    eventLabel_->setObjectName("eventBox");
    eventLabel_->setWordWrap(true);

    auto* tabs = new QTabWidget();
    tabs->addTab(createDashboardTab(), "대시보드");
    tabs->addTab(createConsultationTab(), "상담");
    tabs->addTab(createStudentsTab(), "학생");
    tabs->addTab(createTeachersTab(), "강사");
    tabs->addTab(createTrainingTab(), "트레이닝");
    tabs->addTab(createFacilitiesTab(), "시설");
    tabs->addTab(createFinanceTab(), "재정");
    tabs->addTab(createRivalTab(), "경쟁학원");

    root->addLayout(topBar);
    root->addWidget(eventLabel_);
    root->addWidget(tabs, 1);

    return page;
}

QWidget* MainWindow::createMetricCard(const QString& titleText, QLabel*& valueLabel)
{
    auto* card = new QFrame();
    card->setObjectName("metricCard");

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 13, 16, 13);

    auto* title = new QLabel(titleText);
    title->setObjectName("metricTitle");

    valueLabel = new QLabel("-");
    valueLabel->setObjectName("metricValue");

    layout->addWidget(title);
    layout->addWidget(valueLabel);
    layout->addStretch();

    return card;
}

QWidget* MainWindow::createDashboardTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(16);

    auto* metrics = new QGridLayout();
    metrics->setSpacing(10);

    metrics->addWidget(createMetricCard("보유 자금", cashLabel_), 0, 0);
    metrics->addWidget(createMetricCard("재학생", studentCountLabel_), 0, 1);
    metrics->addWidget(createMetricCard("학원 평판", reputationLabel_), 0, 2);
    metrics->addWidget(createMetricCard("평균 성적", averageScoreLabel_), 0, 3);
    metrics->addWidget(createMetricCard("평균 만족도", averageSatisfactionLabel_), 1, 0);
    metrics->addWidget(createMetricCard("평균 스트레스", averageStressLabel_), 1, 1);
    metrics->addWidget(createMetricCard("현재 정원", capacityLabel_), 1, 2);
    metrics->addWidget(createMetricCard("주간 손익", weeklyNetLabel_), 1, 3);

    auto* statusCard = new QFrame();
    statusCard->setObjectName("card");
    auto* status = new QGridLayout(statusCard);
    status->setContentsMargins(18, 18, 18, 18);
    status->setHorizontalSpacing(18);
    status->setVerticalSpacing(12);

    reputationBar_ = new QProgressBar();
    reputationBar_->setRange(0, 100);

    scoreBar_ = new QProgressBar();
    scoreBar_->setRange(0, 100);

    satisfactionBar_ = new QProgressBar();
    satisfactionBar_->setRange(0, 100);

    stressBar_ = new QProgressBar();
    stressBar_->setRange(0, 100);

    ownerStatusLabel_ = new QLabel();
    ownerStatusLabel_->setWordWrap(true);

    status->addWidget(new QLabel("평판"), 0, 0);
    status->addWidget(reputationBar_, 0, 1);
    status->addWidget(new QLabel("성적"), 1, 0);
    status->addWidget(scoreBar_, 1, 1);
    status->addWidget(new QLabel("만족도"), 2, 0);
    status->addWidget(satisfactionBar_, 2, 1);
    status->addWidget(new QLabel("스트레스"), 3, 0);
    status->addWidget(stressBar_, 3, 1);
    status->addWidget(new QLabel("운영 상황"), 4, 0);
    status->addWidget(ownerStatusLabel_, 4, 1);

    layout->addLayout(metrics);
    layout->addWidget(statusCard);
    layout->addStretch();

    return page;
}

QWidget* MainWindow::createConsultationTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    auto* title = new QLabel("신규 상담");
    title->setObjectName("sectionTitle");

    auto* guide = new QLabel(
        "학생의 현재 성적만 보지 않고 잠재력, 성실성, 집중력, 스트레스와 학부모 요구 수준까지 확인해 등록 여부를 결정합니다."
    );
    guide->setWordWrap(true);
    guide->setObjectName("subtitle");

    applicantTable_ = new QTableWidget();
    applicantTable_->setColumnCount(11);
    applicantTable_->setHorizontalHeaderLabels({
        "이름", "학년", "희망과목", "성적", "잠재력", "성실",
        "집중", "스트레스", "상담적합", "학부모요구", "상담메모"
    });
    applicantTable_->setAlternatingRowColors(true);
    applicantTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    applicantTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    applicantTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    applicantTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    applicantTable_->horizontalHeader()->setStretchLastSection(true);
    applicantTable_->verticalHeader()->setVisible(false);

    auto* buttons = new QHBoxLayout();

    auto* acceptButton = new QPushButton("선택 학생 등록");
    auto* rejectButton = new QPushButton("등록 거절");
    rejectButton->setObjectName("dangerButton");

    connect(acceptButton, &QPushButton::clicked, this, [this] {
        acceptSelectedApplicant();
    });

    connect(rejectButton, &QPushButton::clicked, this, [this] {
        rejectSelectedApplicant();
    });

    buttons->addStretch();
    buttons->addWidget(rejectButton);
    buttons->addWidget(acceptButton);

    layout->addWidget(title);
    layout->addWidget(guide);
    layout->addWidget(applicantTable_, 1);
    layout->addLayout(buttons);

    return page;
}

QWidget* MainWindow::createStudentsTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);

    auto* title = new QLabel("학생 관리");
    title->setObjectName("sectionTitle");

    studentTable_ = new QTableWidget();
    studentTable_->setColumnCount(14);
    studentTable_->setHorizontalHeaderLabels({
        "이름", "과목", "학년", "성적", "잠재력", "성실",
        "이해력", "암기력", "집중력", "시험적응", "스트레스",
        "피로", "만족도", "상태"
    });
    studentTable_->setAlternatingRowColors(true);
    studentTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    studentTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    studentTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    studentTable_->horizontalHeader()->setStretchLastSection(true);
    studentTable_->verticalHeader()->setVisible(false);

    layout->addWidget(title);
    layout->addWidget(studentTable_, 1);

    return page;
}

QWidget* MainWindow::createTeachersTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    auto* currentTitle = new QLabel("현재 강사진");
    currentTitle->setObjectName("sectionTitle");

    teacherTable_ = new QTableWidget();
    teacherTable_->setColumnCount(8);
    teacherTable_->setHorizontalHeaderLabels({
        "이름", "과목", "강의력", "상담", "관리", "스타성", "체력", "특징"
    });
    teacherTable_->setMaximumHeight(230);
    teacherTable_->setAlternatingRowColors(true);
    teacherTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    teacherTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    teacherTable_->verticalHeader()->setVisible(false);

    auto* marketTitle = new QLabel("채용 시장");
    marketTitle->setObjectName("sectionTitle");

    teacherCandidateTable_ = new QTableWidget();
    teacherCandidateTable_->setColumnCount(10);
    teacherCandidateTable_->setHorizontalHeaderLabels({
        "이름", "과목", "강의력", "상담", "관리", "스타성", "체력",
        "특징", "월급", "계약금"
    });
    teacherCandidateTable_->setAlternatingRowColors(true);
    teacherCandidateTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    teacherCandidateTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    teacherCandidateTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    teacherCandidateTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    teacherCandidateTable_->horizontalHeader()->setStretchLastSection(true);
    teacherCandidateTable_->verticalHeader()->setVisible(false);

    auto* hireButton = new QPushButton("선택 강사 채용");
    connect(hireButton, &QPushButton::clicked, this, [this] {
        hireSelectedTeacher();
    });

    auto* buttonRow = new QHBoxLayout();
    buttonRow->addStretch();
    buttonRow->addWidget(hireButton);

    layout->addWidget(currentTitle);
    layout->addWidget(teacherTable_);
    layout->addWidget(marketTitle);
    layout->addWidget(teacherCandidateTable_, 1);
    layout->addLayout(buttonRow);

    return page;
}

QWidget* MainWindow::createTrainingTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(16);

    auto* title = new QLabel("주간 트레이닝 계획");
    title->setObjectName("sectionTitle");

    trainingCombo_ = new QComboBox();
    trainingCombo_->addItem("균형 운영", static_cast<int>(TrainingPlan::Balanced));
    trainingCombo_->addItem("성적 집중", static_cast<int>(TrainingPlan::ScoreIntensive));
    trainingCombo_->addItem("기초 강화", static_cast<int>(TrainingPlan::Fundamentals));
    trainingCombo_->addItem("컨디션 관리", static_cast<int>(TrainingPlan::Care));

    trainingDescriptionLabel_ = new QLabel();
    trainingDescriptionLabel_->setWordWrap(true);

    auto* card = new QFrame();
    card->setObjectName("card");
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(22, 22, 22, 22);
    cardLayout->setSpacing(14);
    cardLayout->addWidget(new QLabel("이번 주 운영 방침"));
    cardLayout->addWidget(trainingCombo_);
    cardLayout->addWidget(trainingDescriptionLabel_);

    auto* note = new QLabel(
        "상단의 '1주 진행' 또는 '4주 진행'을 누르면 현재 선택된 트레이닝 방침이 적용됩니다. "
        "성적만 밀어붙이면 스트레스와 피로가 누적되고, 관리 위주로 운영하면 성장 속도가 느려질 수 있습니다."
    );
    note->setWordWrap(true);
    note->setObjectName("subtitle");

    connect(trainingCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] {
        refreshTrainingDescription();
    });

    layout->addWidget(title);
    layout->addWidget(card);
    layout->addWidget(note);
    layout->addStretch();

    refreshTrainingDescription();
    return page;
}

QWidget* MainWindow::createFacilitiesTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    auto* title = new QLabel("시설 및 확장");
    title->setObjectName("sectionTitle");

    auto* grid = new QGridLayout();
    grid->setSpacing(12);

    auto makeFacilityCard = [this](
        const QString& titleText,
        QLabel*& levelLabel,
        QLabel*& costLabel,
        const QString& buttonText,
        const std::function<void()>& action
    ) -> QWidget*
    {
        auto* card = new QFrame();
        card->setObjectName("card");
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 18, 18, 18);

        auto* titleLabel = new QLabel(titleText);
        titleLabel->setObjectName("sectionTitle");

        levelLabel = new QLabel();
        costLabel = new QLabel();

        auto* button = new QPushButton(buttonText);
        button->setObjectName("facilityButton");

        connect(button, &QPushButton::clicked, this, [this, action] {
            action();
            refreshAll();
        });

        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(levelLabel);
        cardLayout->addWidget(costLabel);
        cardLayout->addStretch();
        cardLayout->addWidget(button);

        return card;
    };

    grid->addWidget(
        makeFacilityCard(
            "강의실",
            classroomLabel_,
            classroomCostLabel_,
            "강의실 확장",
            [this] { simulation_.expandClassroom(); }
        ),
        0,
        0
    );

    grid->addWidget(
        makeFacilityCard(
            "인테리어",
            interiorLabel_,
            interiorCostLabel_,
            "인테리어 개선",
            [this] { simulation_.upgradeInterior(); }
        ),
        0,
        1
    );

    grid->addWidget(
        makeFacilityCard(
            "자습실",
            studyRoomLabel_,
            studyRoomCostLabel_,
            "자습실 개선",
            [this] { simulation_.upgradeStudyRoom(); }
        ),
        0,
        2
    );

    facilityCapacityLabel_ = new QLabel();
    facilityCapacityLabel_->setObjectName("subtitle");

    layout->addWidget(title);
    layout->addLayout(grid);
    layout->addWidget(facilityCapacityLabel_);
    layout->addStretch();

    return page;
}

QWidget* MainWindow::createFinanceTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    auto* title = new QLabel("주간 재정");
    title->setObjectName("sectionTitle");

    auto* panel = new QFrame();
    panel->setObjectName("card");

    auto* grid = new QGridLayout(panel);
    grid->setContentsMargins(22, 22, 22, 22);
    grid->setHorizontalSpacing(30);
    grid->setVerticalSpacing(15);

    revenueLabel_ = new QLabel();
    payrollLabel_ = new QLabel();
    rentLabel_ = new QLabel();
    electricityLabel_ = new QLabel();
    waterLabel_ = new QLabel();
    maintenanceLabel_ = new QLabel();
    financeNetLabel_ = new QLabel();

    grid->addWidget(new QLabel("수강료 매출"), 0, 0);
    grid->addWidget(revenueLabel_, 0, 1);
    grid->addWidget(new QLabel("강사 급여"), 1, 0);
    grid->addWidget(payrollLabel_, 1, 1);
    grid->addWidget(new QLabel("임대료"), 2, 0);
    grid->addWidget(rentLabel_, 2, 1);
    grid->addWidget(new QLabel("전기세"), 3, 0);
    grid->addWidget(electricityLabel_, 3, 1);
    grid->addWidget(new QLabel("수도세"), 4, 0);
    grid->addWidget(waterLabel_, 4, 1);
    grid->addWidget(new QLabel("시설 유지비"), 5, 0);
    grid->addWidget(maintenanceLabel_, 5, 1);
    grid->addWidget(new QLabel("주간 순손익"), 6, 0);
    grid->addWidget(financeNetLabel_, 6, 1);

    auto* note = new QLabel(
        "강의실과 인테리어를 확장하면 정원과 평판은 좋아지지만 임대료·전기세·유지비도 함께 올라갑니다. "
        "강사를 늘릴수록 과목 대응력은 좋아지지만 고정 급여가 증가합니다."
    );
    note->setWordWrap(true);
    note->setObjectName("subtitle");

    layout->addWidget(title);
    layout->addWidget(panel);
    layout->addWidget(note);
    layout->addStretch();

    return page;
}

QWidget* MainWindow::createRivalTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    auto* title = new QLabel("옆 학원 경쟁");
    title->setObjectName("sectionTitle");

    auto* panel = new QFrame();
    panel->setObjectName("card");
    auto* grid = new QGridLayout(panel);
    grid->setContentsMargins(22, 22, 22, 22);
    grid->setHorizontalSpacing(26);
    grid->setVerticalSpacing(14);

    rivalNameLabel_ = new QLabel();
    rivalSpecialtyLabel_ = new QLabel();
    rivalReputationLabel_ = new QLabel();
    rivalStudentsLabel_ = new QLabel();
    rivalrySummaryLabel_ = new QLabel();
    rivalrySummaryLabel_->setWordWrap(true);

    grid->addWidget(new QLabel("경쟁 학원"), 0, 0);
    grid->addWidget(rivalNameLabel_, 0, 1);
    grid->addWidget(new QLabel("주력 과목"), 1, 0);
    grid->addWidget(rivalSpecialtyLabel_, 1, 1);
    grid->addWidget(new QLabel("평판"), 2, 0);
    grid->addWidget(rivalReputationLabel_, 2, 1);
    grid->addWidget(new QLabel("재학생"), 3, 0);
    grid->addWidget(rivalStudentsLabel_, 3, 1);
    grid->addWidget(new QLabel("현재 구도"), 4, 0);
    grid->addWidget(rivalrySummaryLabel_, 4, 1);

    auto* note = new QLabel(
        "경쟁 학원은 매주 학생 수와 평판이 변합니다. 추후 광고, 강사 스카우트, 시험 성과, 지역 점유율과 연결할 수 있도록 분리된 상태값으로 구성했습니다."
    );
    note->setWordWrap(true);
    note->setObjectName("subtitle");

    layout->addWidget(title);
    layout->addWidget(panel);
    layout->addWidget(note);
    layout->addStretch();

    return page;
}

void MainWindow::editIntro()
{
    QDialog dialog(this);
    dialog.setWindowTitle("인트로 편집");
    dialog.resize(720, 520);

    auto* layout = new QVBoxLayout(&dialog);

    auto* guide = new QLabel(
        "아래 내용을 그대로 타이핑해 수정하면 됩니다. C++ 코드를 건드릴 필요가 없습니다."
    );
    guide->setWordWrap(true);

    auto* editor = new QTextEdit();
    editor->setPlainText(introBrowser_->toPlainText());

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel
    );

    auto* resetButton = new QPushButton("기본 문구로 되돌리기");
    resetButton->setObjectName("secondaryButton");
    buttons->addButton(resetButton, QDialogButtonBox::ResetRole);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    connect(resetButton, &QPushButton::clicked, this, [this, editor] {
        editor->setPlainText(defaultIntroText());
    });

    layout->addWidget(guide);
    layout->addWidget(editor, 1);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted)
    {
        const QString text = editor->toPlainText().trimmed().isEmpty()
            ? defaultIntroText()
            : editor->toPlainText();

        introBrowser_->setPlainText(text);

        QSettings settings("Skylark51", "HakwonKeewoogi");
        settings.setValue("intro/text", text);
    }
}

void MainWindow::beginSetup()
{
    rootStack_->setCurrentWidget(setupPage_);
    academyNameEdit_->setFocus();
    academyNameEdit_->selectAll();
}

void MainWindow::startGame()
{
    simulation_.setupAcademy(
        academyNameEdit_->text(),
        specialtyCombo_->currentText()
    );

    academyTitleLabel_->setText(simulation_.academyName());
    rootStack_->setCurrentWidget(gamePage_);
    refreshAll();
}

void MainWindow::acceptSelectedApplicant()
{
    const int row = applicantTable_->currentRow();
    if (row < 0)
        return;

    simulation_.acceptApplicant(row);
    refreshAll();
}

void MainWindow::rejectSelectedApplicant()
{
    const int row = applicantTable_->currentRow();
    if (row < 0)
        return;

    simulation_.rejectApplicant(row);
    refreshAll();
}

void MainWindow::hireSelectedTeacher()
{
    const int row = teacherCandidateTable_->currentRow();
    if (row < 0)
        return;

    simulation_.hireTeacher(row);
    refreshAll();
}

TrainingPlan MainWindow::selectedTrainingPlan() const
{
    if (!trainingCombo_)
        return TrainingPlan::Balanced;

    return static_cast<TrainingPlan>(trainingCombo_->currentData().toInt());
}

void MainWindow::advanceWeeks(int count)
{
    const TrainingPlan plan = selectedTrainingPlan();

    for (int i = 0; i < count; ++i)
        simulation_.advanceWeek(plan);

    refreshAll();
}

void MainWindow::refreshAll()
{
    academyTitleLabel_->setText(
        QString("%1 · %2 중심").arg(simulation_.academyName(), simulation_.specialty())
    );
    dateLabel_->setText(simulation_.dateText());
    eventLabel_->setText(simulation_.latestEvent());

    cashLabel_->setText(money(simulation_.cash()));
    studentCountLabel_->setText(QString("%1명").arg(simulation_.students().size()));
    reputationLabel_->setText(QString::number(simulation_.reputation()));
    averageScoreLabel_->setText(QString::number(simulation_.averageScore(), 'f', 1));
    averageSatisfactionLabel_->setText(
        QString::number(simulation_.averageSatisfaction(), 'f', 1)
    );
    averageStressLabel_->setText(
        QString::number(simulation_.averageStress(), 'f', 1)
    );

    capacityLabel_->setText(
        QString("%1 / %2명")
            .arg(simulation_.students().size())
            .arg(simulation_.facilities().capacity())
    );

    weeklyNetLabel_->setText(money(simulation_.lastFinance().net));

    reputationBar_->setValue(simulation_.reputation());
    scoreBar_->setValue(static_cast<int>(simulation_.averageScore()));
    satisfactionBar_->setValue(static_cast<int>(simulation_.averageSatisfaction()));
    stressBar_->setValue(static_cast<int>(simulation_.averageStress()));

    if (simulation_.teachers().isEmpty())
    {
        ownerStatusLabel_->setText(
            QString("강사 0명. 원장이 %1 과목을 직접 담당하고 있습니다. 비주력 과목은 수업 효율이 낮습니다.")
                .arg(simulation_.specialty())
        );
    }
    else
    {
        ownerStatusLabel_->setText(
            QString("강사 %1명 고용 중. 원장은 빈 과목을 보조하고 학원 운영을 총괄합니다.")
                .arg(simulation_.teachers().size())
        );
    }

    refreshApplicants();
    refreshStudents();
    refreshTeachers();
    refreshFacilities();
    refreshFinance();
    refreshRival();
    refreshTrainingDescription();
}

void MainWindow::refreshApplicants()
{
    const auto& applicants = simulation_.applicants();
    applicantTable_->setRowCount(static_cast<int>(applicants.size()));

    for (int row = 0; row < applicants.size(); ++row)
    {
        const Applicant& applicant = applicants.at(row);
        const Student& student = applicant.student;

        applicantTable_->setItem(row, 0, centeredItem(student.name));
        applicantTable_->setItem(row, 1, centeredItem(QString("%1학년").arg(student.schoolYear)));
        applicantTable_->setItem(row, 2, centeredItem(student.subject));
        applicantTable_->setItem(row, 3, centeredItem(QString::number(student.score, 'f', 1)));
        applicantTable_->setItem(row, 4, centeredItem(QString::number(student.potential)));
        applicantTable_->setItem(row, 5, centeredItem(QString::number(student.diligence)));
        applicantTable_->setItem(row, 6, centeredItem(QString::number(student.concentration)));
        applicantTable_->setItem(row, 7, centeredItem(QString::number(student.stress, 'f', 0)));
        applicantTable_->setItem(row, 8, centeredItem(QString::number(applicant.consultationFit)));
        applicantTable_->setItem(row, 9, centeredItem(QString::number(applicant.parentDemand)));
        applicantTable_->setItem(row, 10, centeredItem(applicant.note));
    }
}

void MainWindow::refreshStudents()
{
    const auto& students = simulation_.students();
    studentTable_->setRowCount(static_cast<int>(students.size()));

    for (int row = 0; row < students.size(); ++row)
    {
        const Student& student = students.at(row);

        studentTable_->setItem(row, 0, centeredItem(student.name));
        studentTable_->setItem(row, 1, centeredItem(student.subject));
        studentTable_->setItem(row, 2, centeredItem(QString("%1학년").arg(student.schoolYear)));
        studentTable_->setItem(row, 3, centeredItem(QString::number(student.score, 'f', 1)));
        studentTable_->setItem(row, 4, centeredItem(QString::number(student.potential)));
        studentTable_->setItem(row, 5, centeredItem(QString::number(student.diligence)));
        studentTable_->setItem(row, 6, centeredItem(QString::number(student.comprehension)));
        studentTable_->setItem(row, 7, centeredItem(QString::number(student.memorization)));
        studentTable_->setItem(row, 8, centeredItem(QString::number(student.concentration)));
        studentTable_->setItem(row, 9, centeredItem(QString::number(student.examNerves)));
        studentTable_->setItem(row, 10, centeredItem(QString::number(student.stress, 'f', 1)));
        studentTable_->setItem(row, 11, centeredItem(QString::number(student.fatigue, 'f', 1)));
        studentTable_->setItem(row, 12, centeredItem(QString::number(student.satisfaction, 'f', 1)));
        studentTable_->setItem(row, 13, centeredItem(student.conditionText()));
    }
}

void MainWindow::refreshTeachers()
{
    const auto& teachers = simulation_.teachers();
    teacherTable_->setRowCount(static_cast<int>(teachers.size()));

    for (int row = 0; row < teachers.size(); ++row)
    {
        const Teacher& teacher = teachers.at(row);

        teacherTable_->setItem(row, 0, centeredItem(teacher.name));
        teacherTable_->setItem(row, 1, centeredItem(teacher.subject));
        teacherTable_->setItem(row, 2, centeredItem(QString::number(teacher.teaching)));
        teacherTable_->setItem(row, 3, centeredItem(QString::number(teacher.counseling)));
        teacherTable_->setItem(row, 4, centeredItem(QString::number(teacher.discipline)));
        teacherTable_->setItem(row, 5, centeredItem(QString::number(teacher.charisma)));
        teacherTable_->setItem(row, 6, centeredItem(QString::number(teacher.stamina)));
        teacherTable_->setItem(row, 7, centeredItem(teacher.trait));
    }

    const auto& candidates = simulation_.teacherCandidates();
    teacherCandidateTable_->setRowCount(static_cast<int>(candidates.size()));

    for (int row = 0; row < candidates.size(); ++row)
    {
        const Teacher& teacher = candidates.at(row);

        teacherCandidateTable_->setItem(row, 0, centeredItem(teacher.name));
        teacherCandidateTable_->setItem(row, 1, centeredItem(teacher.subject));
        teacherCandidateTable_->setItem(row, 2, centeredItem(QString::number(teacher.teaching)));
        teacherCandidateTable_->setItem(row, 3, centeredItem(QString::number(teacher.counseling)));
        teacherCandidateTable_->setItem(row, 4, centeredItem(QString::number(teacher.discipline)));
        teacherCandidateTable_->setItem(row, 5, centeredItem(QString::number(teacher.charisma)));
        teacherCandidateTable_->setItem(row, 6, centeredItem(QString::number(teacher.stamina)));
        teacherCandidateTable_->setItem(row, 7, centeredItem(teacher.trait));
        teacherCandidateTable_->setItem(row, 8, centeredItem(money(teacher.monthlySalary)));
        teacherCandidateTable_->setItem(row, 9, centeredItem(money(teacher.signingCost)));
    }
}

void MainWindow::refreshFacilities()
{
    const FacilityState& f = simulation_.facilities();

    classroomLabel_->setText(QString("강의실 %1개").arg(f.classrooms));
    interiorLabel_->setText(QString("인테리어 Lv.%1").arg(f.interiorLevel));
    studyRoomLabel_->setText(QString("자습실 Lv.%1").arg(f.studyRoomLevel));

    interiorCostLabel_->setText(
        QString("다음 개선 비용: %1").arg(money(simulation_.interiorUpgradeCost()))
    );
    classroomCostLabel_->setText(
        QString("다음 확장 비용: %1").arg(money(simulation_.classroomExpansionCost()))
    );
    studyRoomCostLabel_->setText(
        QString("다음 개선 비용: %1").arg(money(simulation_.studyRoomUpgradeCost()))
    );

    facilityCapacityLabel_->setText(
        QString("현재 수용 인원: %1 / %2명 · 강의실을 늘리면 정원이 증가하고, 인테리어와 자습실은 평판·만족도에 영향을 줍니다.")
            .arg(simulation_.students().size())
            .arg(f.capacity())
    );
}

void MainWindow::refreshFinance()
{
    const FinanceSnapshot& finance = simulation_.lastFinance();

    revenueLabel_->setText(money(finance.revenue));
    payrollLabel_->setText("- " + money(finance.payroll));
    rentLabel_->setText("- " + money(finance.rent));
    electricityLabel_->setText("- " + money(finance.electricity));
    waterLabel_->setText("- " + money(finance.water));
    maintenanceLabel_->setText("- " + money(finance.maintenance));
    financeNetLabel_->setText(money(finance.net));
}

void MainWindow::refreshRival()
{
    const RivalAcademy& rival = simulation_.rival();

    rivalNameLabel_->setText(rival.name);
    rivalSpecialtyLabel_->setText(rival.specialty);
    rivalReputationLabel_->setText(QString::number(rival.reputation));
    rivalStudentsLabel_->setText(QString("%1명").arg(rival.students));

    const int reputationGap = simulation_.reputation() - rival.reputation;
    const int studentGap = static_cast<int>(simulation_.students().size()) - rival.students;

    QString summary;

    if (reputationGap >= 8)
        summary = "평판 경쟁에서는 우리 학원이 앞서고 있습니다.";
    else if (reputationGap <= -8)
        summary = "평판에서 경쟁 학원에 밀리고 있습니다. 상담 유입에 불리할 수 있습니다.";
    else
        summary = "평판 경쟁은 비슷한 수준입니다.";

    summary += QString(" 재학생 차이는 %1명입니다.").arg(studentGap);

    rivalrySummaryLabel_->setText(summary);
}

void MainWindow::refreshTrainingDescription()
{
    if (!trainingDescriptionLabel_ || !trainingCombo_)
        return;

    switch (selectedTrainingPlan())
    {
    case TrainingPlan::ScoreIntensive:
        trainingDescriptionLabel_->setText(
            "성적 상승 효율이 가장 높습니다. 대신 스트레스와 피로가 빠르게 쌓이고 만족도가 하락할 수 있습니다."
        );
        break;
    case TrainingPlan::Fundamentals:
        trainingDescriptionLabel_->setText(
            "성장 속도는 조금 느리지만 안정적으로 성적을 끌어올립니다. 기초가 약한 학생을 장기 육성할 때 적합합니다."
        );
        break;
    case TrainingPlan::Care:
        trainingDescriptionLabel_->setText(
            "성적 상승을 일부 포기하고 스트레스와 피로를 낮추며 만족도를 회복합니다. 위험 상태 학생이 많을 때 유리합니다."
        );
        break;
    case TrainingPlan::Balanced:
    default:
        trainingDescriptionLabel_->setText(
            "성적 성장과 컨디션을 모두 고려하는 기본 운영입니다. 특별한 문제가 없을 때 가장 안정적입니다."
        );
        break;
    }
}

QString MainWindow::defaultIntroText() const
{
    return
        "몇 년 동안 남의 학원에서 강의하고, 상담하고, 학생을 붙잡아 왔다.\n\n"
        "이제는 내 이름으로 된 학원을 열기로 했다.\n"
        "좋은 학생만 골라 받을 수도 없고, 좋은 강사를 무작정 데려올 돈도 없다.\n"
        "성적을 올리려면 학생을 밀어붙여야 하지만, 너무 밀어붙이면 학생이 먼저 지친다.\n"
        "시설을 넓히면 더 많은 학생을 받을 수 있지만 월세와 공과금도 따라 올라간다.\n"
        "게다가 바로 옆에는 이미 자리를 잡은 경쟁 학원이 있다.\n\n"
        "처음 가진 것은 작은 강의실 하나와 3,500만원.\n"
        "그리고 내가 직접 가르칠 수 있는 한 과목뿐이다.\n\n"
        "어떤 학생을 받을 것인가.\n"
        "누구를 강사로 고용할 것인가.\n"
        "성적과 학생 상태, 돈과 평판 사이에서 어디까지 밀어붙일 것인가.\n\n"
        "학원 운영을 시작한다.";
}

QString MainWindow::money(qint64 value)
{
    const bool negative = value < 0;
    const quint64 absolute =
        negative ? static_cast<quint64>(-value) : static_cast<quint64>(value);

    QString digits = QString::number(absolute);

    for (int i = digits.size() - 3; i > 0; i -= 3)
        digits.insert(i, ',');

    return QString("%1₩%2")
        .arg(negative ? "-" : "")
        .arg(digits);
}

QTableWidgetItem* MainWindow::centeredItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}
