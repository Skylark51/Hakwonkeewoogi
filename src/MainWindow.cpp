#include "MainWindow.h"

#include "AcademySceneWidget.h"

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
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabBar>
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
    setWindowTitle("학원키우기 v0.3");
    resize(1460, 900);
    setMinimumSize(1180, 760);

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
            font-size: 21px;
            font-weight: 800;
        }

        QLabel#eventBox {
            background: #fff8df;
            border: 1px solid #e4d49b;
            border-radius: 8px;
            padding: 10px;
            color: #4b4120;
        }

        QLabel#assessmentBox {
            background: #eaf1ff;
            border: 1px solid #b9caed;
            border-radius: 6px;
            padding: 7px 10px;
            color: #263e68;
            font-weight: 700;
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

        QPushButton:disabled {
            background: #98a3b2;
            color: #e9edf2;
        }

        QPushButton#secondaryButton {
            background: #e7ebf0;
            color: #26374f;
            border: 1px solid #d0d6de;
        }

        QPushButton#dangerButton {
            background: #8f3b3b;
        }

        QPushButton#classStartButton {
            background: #47754b;
            font-size: 15px;
            padding: 11px 20px;
        }

        QPushButton#facilityButton {
            background: #52667f;
        }

        QLineEdit, QComboBox, QSpinBox, QTextEdit, QTextBrowser {
            background: white;
            border: 1px solid #ccd3dc;
            border-radius: 6px;
            padding: 8px;
        }

        QTextBrowser#introBox {
            font-size: 15px;
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
            padding: 10px 14px;
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
        "작은 단과학원에서 시작해 학생, 강사, 시설, 수강료와 지역 경쟁을 관리하는 경영 시뮬레이션"
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
        "처음에는 원장 혼자 한 과목만 가르칩니다. 다른 과목 학생을 받으려면 해당 과목 강사를 채용한 뒤 과목을 개설해야 합니다."
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
    specialtyCombo_->addItems({"수학", "영어", "국어", "과학", "사회"});

    form->addWidget(new QLabel("학원 이름"), 0, 0);
    form->addWidget(academyNameEdit_, 0, 1);
    form->addWidget(new QLabel("원장 주력 과목"), 1, 0);
    form->addWidget(specialtyCombo_, 1, 1);

    auto* guide = new QLabel(
        "시작 자금 3,500만원 · 강의실 1개 · 정원 18명 · 강사 0명\n"
        "초기 상담 학생은 주력 과목만 등장하며, 첫 상담 풀에는 잠재력이 높은 학생 1명이 반드시 포함됩니다."
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
    root->setContentsMargins(16, 14, 16, 16);
    root->setSpacing(10);

    auto* topBar = new QHBoxLayout();

    academyTitleLabel_ = new QLabel("학원키우기");
    academyTitleLabel_->setObjectName("gameTitle");

    dateLabel_ = new QLabel();

    assessmentLabel_ = new QLabel();
    assessmentLabel_->setObjectName("assessmentBox");

    classStartButton_ = new QPushButton("강의 시작 · 1주 진행");
    classStartButton_->setObjectName("classStartButton");
    classStartButton_->setMinimumWidth(190);

    connect(classStartButton_, &QPushButton::clicked, this, [this] {
        startClassWeek();
    });

    topBar->addWidget(academyTitleLabel_);
    topBar->addSpacing(16);
    topBar->addWidget(dateLabel_);
    topBar->addSpacing(10);
    topBar->addWidget(assessmentLabel_);
    topBar->addStretch();
    topBar->addWidget(classStartButton_);

    eventLabel_ = new QLabel();
    eventLabel_->setObjectName("eventBox");
    eventLabel_->setWordWrap(true);

    gameTabs_ = new QTabWidget();
    gameTabs_->addTab(createDashboardTab(), "학원");
    gameTabs_->addTab(createConsultationTab(), "상담");
    gameTabs_->addTab(createStudentsTab(), "학생");
    gameTabs_->addTab(createTeachersTab(), "강사");
    gameTabs_->addTab(createCoursesTab(), "과목·수강료");
    gameTabs_->addTab(createTrainingTab(), "트레이닝");
    gameTabs_->addTab(createFacilitiesTab(), "시설");
    gameTabs_->addTab(createFinanceTab(), "재정");
    gameTabs_->addTab(createRivalTab(), "경쟁학원");

    root->addLayout(topBar);
    root->addWidget(eventLabel_);
    root->addWidget(gameTabs_, 1);

    return page;
}

QWidget* MainWindow::createMetricCard(const QString& titleText, QLabel*& valueLabel)
{
    auto* card = new QFrame();
    card->setObjectName("metricCard");

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(13, 10, 13, 10);

    auto* title = new QLabel(titleText);
    title->setObjectName("metricTitle");

    valueLabel = new QLabel("-");
    valueLabel->setObjectName("metricValue");

    layout->addWidget(title);
    layout->addWidget(valueLabel);

    return card;
}

QWidget* MainWindow::createDashboardTab()
{
    auto* page = new QWidget();

    auto* root = new QHBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(12);

    academyScene_ = new AcademySceneWidget();
    academyScene_->setSimulation(&simulation_);

    connect(
        academyScene_,
        &AcademySceneWidget::classAnimationFinished,
        this,
        [this] { finishClassWeek(); }
    );

    auto* side = new QWidget();
    side->setMinimumWidth(330);
    side->setMaximumWidth(390);

    auto* sideLayout = new QVBoxLayout(side);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(10);

    auto* metrics = new QGridLayout();
    metrics->setSpacing(8);

    metrics->addWidget(createMetricCard("보유 자금", cashLabel_), 0, 0);
    metrics->addWidget(createMetricCard("재학생", studentCountLabel_), 0, 1);
    metrics->addWidget(createMetricCard("학원 평판", reputationLabel_), 1, 0);
    metrics->addWidget(createMetricCard("학업능력", averageSkillLabel_), 1, 1);
    metrics->addWidget(createMetricCard("최근 시험", averageExamLabel_), 2, 0);
    metrics->addWidget(createMetricCard("주간 손익", weeklyNetLabel_), 2, 1);
    metrics->addWidget(createMetricCard("만족도", averageSatisfactionLabel_), 3, 0);
    metrics->addWidget(createMetricCard("스트레스", averageStressLabel_), 3, 1);
    metrics->addWidget(createMetricCard("정원", capacityLabel_), 4, 0, 1, 2);

    auto* statusCard = new QFrame();
    statusCard->setObjectName("card");

    auto* status = new QGridLayout(statusCard);
    status->setContentsMargins(14, 14, 14, 14);
    status->setVerticalSpacing(8);

    reputationBar_ = new QProgressBar();
    reputationBar_->setRange(0, 100);

    skillBar_ = new QProgressBar();
    skillBar_->setRange(0, 100);

    satisfactionBar_ = new QProgressBar();
    satisfactionBar_->setRange(0, 100);

    stressBar_ = new QProgressBar();
    stressBar_->setRange(0, 100);

    ownerStatusLabel_ = new QLabel();
    ownerStatusLabel_->setWordWrap(true);

    status->addWidget(new QLabel("평판"), 0, 0);
    status->addWidget(reputationBar_, 0, 1);
    status->addWidget(new QLabel("학업"), 1, 0);
    status->addWidget(skillBar_, 1, 1);
    status->addWidget(new QLabel("만족"), 2, 0);
    status->addWidget(satisfactionBar_, 2, 1);
    status->addWidget(new QLabel("스트레스"), 3, 0);
    status->addWidget(stressBar_, 3, 1);
    status->addWidget(ownerStatusLabel_, 4, 0, 1, 2);

    sideLayout->addLayout(metrics);
    sideLayout->addWidget(statusCard);
    sideLayout->addStretch();

    root->addWidget(academyScene_, 1);
    root->addWidget(side);

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
        "개설된 과목의 학생만 상담 요청이 옵니다. 잠재력 90 이상 학생은 희귀하며, 첫 게임 시작 시에만 한 명을 보장합니다."
    );
    guide->setWordWrap(true);
    guide->setObjectName("subtitle");

    applicantTable_ = new QTableWidget();
    applicantTable_->setColumnCount(12);
    applicantTable_->setHorizontalHeaderLabels({
        "이름", "학년", "과목", "월수강료", "학업", "잠재력",
        "성실", "집중", "스트레스", "상담적합", "학부모요구", "메모"
    });
    applicantTable_->setAlternatingRowColors(true);
    applicantTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    applicantTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    applicantTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    applicantTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    applicantTable_->horizontalHeader()->setStretchLastSection(true);
    applicantTable_->verticalHeader()->setVisible(false);

    auto* buttons = new QHBoxLayout();

    auto* rejectButton = new QPushButton("등록 거절");
    rejectButton->setObjectName("dangerButton");

    auto* acceptButton = new QPushButton("선택 학생 등록");

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
    studentTable_->setColumnCount(15);
    studentTable_->setHorizontalHeaderLabels({
        "이름", "과목", "학년", "학업능력", "최근시험", "잠재력", "성실",
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
    teacherTable_->setColumnCount(9);
    teacherTable_->setHorizontalHeaderLabels({
        "이름", "과목", "강의력", "상담", "관리", "스타성", "체력", "특징", "월급"
    });
    teacherTable_->setMaximumHeight(220);
    teacherTable_->setAlternatingRowColors(true);
    teacherTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    teacherTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    teacherTable_->verticalHeader()->setVisible(false);

    auto* marketTitle = new QLabel("채용 시장");
    marketTitle->setObjectName("sectionTitle");

    auto* guide = new QLabel(
        "다른 과목 강사를 채용해도 과목이 자동 개설되지는 않습니다. 채용 후 '과목·수강료'에서 직접 개설해야 합니다."
    );
    guide->setWordWrap(true);
    guide->setObjectName("subtitle");

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
    layout->addWidget(guide);
    layout->addWidget(teacherCandidateTable_, 1);
    layout->addLayout(buttonRow);

    return page;
}

QWidget* MainWindow::createCoursesTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    auto* title = new QLabel("과목 개설 및 수강료");
    title->setObjectName("sectionTitle");

    tuitionGuideLabel_ = new QLabel(
        "게임 기준 월 수강료(주 3회 운영 가정): 수학 40만 · 영어 40만 · 국어 30만 · 과학 25만 · 사회 25만원. "
        "현재 게임에서는 시장 기준의 60~120% 범위에서 조정할 수 있습니다. 실제 학원 교습비 등록 기준과는 별개의 게임 규칙입니다."
    );
    tuitionGuideLabel_->setWordWrap(true);
    tuitionGuideLabel_->setObjectName("subtitle");

    courseTable_ = new QTableWidget();
    courseTable_->setColumnCount(7);
    courseTable_->setHorizontalHeaderLabels({
        "과목", "개설상태", "담당 가능", "시장기준", "현재 수강료", "조정범위", "가격효과"
    });
    courseTable_->setAlternatingRowColors(true);
    courseTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    courseTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    courseTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    courseTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    courseTable_->verticalHeader()->setVisible(false);

    connect(
        courseTable_,
        &QTableWidget::currentCellChanged,
        this,
        [this](int, int, int, int) {
            syncCourseEditorFromSelection();
        }
    );

    auto* editorCard = new QFrame();
    editorCard->setObjectName("card");

    auto* editorLayout = new QHBoxLayout(editorCard);
    editorLayout->setContentsMargins(16, 16, 16, 16);

    tuitionSpin_ = new QSpinBox();
    tuitionSpin_->setRange(0, 1000000);
    tuitionSpin_->setSingleStep(10000);
    tuitionSpin_->setSuffix(" 원");
    tuitionSpin_->setMinimumWidth(180);

    auto* applyButton = new QPushButton("수강료 적용");

    courseToggleButton_ = new QPushButton("과목 개설");

    connect(applyButton, &QPushButton::clicked, this, [this] {
        applySelectedTuition();
    });

    connect(courseToggleButton_, &QPushButton::clicked, this, [this] {
        toggleSelectedSubject();
    });

    editorLayout->addWidget(new QLabel("선택 과목 월 수강료"));
    editorLayout->addWidget(tuitionSpin_);
    editorLayout->addWidget(applyButton);
    editorLayout->addStretch();
    editorLayout->addWidget(courseToggleButton_);

    layout->addWidget(title);
    layout->addWidget(tuitionGuideLabel_);
    layout->addWidget(courseTable_, 1);
    layout->addWidget(editorCard);

    return page;
}

QWidget* MainWindow::createTrainingTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(16);

    auto* title = new QLabel("이번 주 트레이닝 계획");
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

    cardLayout->addWidget(new QLabel("운영 방침"));
    cardLayout->addWidget(trainingCombo_);
    cardLayout->addWidget(trainingDescriptionLabel_);

    auto* note = new QLabel(
        "학생·강사·과목·수강료·시설 관리를 마친 뒤 상단의 '강의 시작'을 누르면 선택한 방침으로 1주가 진행됩니다."
    );
    note->setWordWrap(true);
    note->setObjectName("subtitle");

    connect(
        trainingCombo_,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        [this] { refreshTrainingDescription(); }
    );

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
    layout->setSpacing(12);

    auto* title = new QLabel("인테리어 · 방 구성 · 확장");
    title->setObjectName("sectionTitle");

    roomUsageLabel_ = new QLabel();
    roomUsageLabel_->setObjectName("subtitle");

    auto* grid = new QGridLayout();
    grid->setSpacing(10);

    auto makeCard = [this](
        const QString& titleText,
        QLabel*& stateLabel,
        const QString& buttonText,
        const std::function<void()>& action
    ) -> QWidget*
    {
        auto* card = new QFrame();
        card->setObjectName("card");

        auto* box = new QVBoxLayout(card);
        box->setContentsMargins(16, 16, 16, 16);

        auto* title = new QLabel(titleText);
        title->setObjectName("sectionTitle");

        stateLabel = new QLabel();
        stateLabel->setWordWrap(true);

        auto* button = new QPushButton(buttonText);
        button->setObjectName("facilityButton");

        connect(button, &QPushButton::clicked, this, [this, action] {
            action();
            refreshAll();
        });

        box->addWidget(title);
        box->addWidget(stateLabel);
        box->addStretch();
        box->addWidget(button);

        return card;
    };

    grid->addWidget(
        makeCard("학원 면적", floorLabel_, "방 넓히기", [this] {
            simulation_.expandFloor();
        }),
        0,
        0
    );

    grid->addWidget(
        makeCard("강의실", classroomLabel_, "강의실 늘리기", [this] {
            simulation_.addClassroom();
        }),
        0,
        1
    );

    grid->addWidget(
        makeCard("원장실", directorOfficeLabel_, "원장실 만들기", [this] {
            simulation_.buildDirectorOffice();
        }),
        0,
        2
    );

    grid->addWidget(
        makeCard("상담실", consultationRoomLabel_, "상담실 만들기", [this] {
            simulation_.buildConsultationRoom();
        }),
        1,
        0
    );

    grid->addWidget(
        makeCard("탕비실", pantryLabel_, "탕비실 만들기", [this] {
            simulation_.buildPantry();
        }),
        1,
        1
    );

    grid->addWidget(
        makeCard("자습실", studyRoomLabel_, "자습실 만들기", [this] {
            simulation_.buildStudyRoom();
        }),
        1,
        2
    );

    grid->addWidget(
        makeCard("인테리어", interiorLabel_, "인테리어 개선", [this] {
            simulation_.upgradeInterior();
        }),
        2,
        0,
        1,
        3
    );

    layout->addWidget(title);
    layout->addWidget(roomUsageLabel_);
    layout->addLayout(grid);
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
        "방과 시설을 늘리면 임대료·전기세·유지비가 함께 증가합니다. 수강료 인상은 학생당 매출을 높이지만 상담 유입·평판·퇴원에 불리하게 작용합니다."
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
        "현재는 평판과 학생 수 경쟁부터 구현되어 있습니다. 이후 동일 상담 학생 경쟁, 광고, 강사 스카우트와 지역 점유율로 확장합니다."
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

    rootStack_->setCurrentWidget(gamePage_);
    gameTabs_->setCurrentIndex(0);

    academyScene_->setSimulation(&simulation_);
    refreshAll();
}

void MainWindow::acceptSelectedApplicant()
{
    const int row = applicantTable_->currentRow();

    if (row < 0)
        return;

    simulation_.acceptApplicant(row);
    academyScene_->refreshScene();
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

void MainWindow::applySelectedTuition()
{
    const int row = courseTable_->currentRow();

    if (row < 0)
        return;

    simulation_.setTuition(row, tuitionSpin_->value());
    refreshAll();
}

void MainWindow::toggleSelectedSubject()
{
    const int row = courseTable_->currentRow();

    if (row < 0)
        return;

    const auto& offers = simulation_.subjectOffers();

    if (row >= offers.size())
        return;

    if (offers.at(row).opened)
        simulation_.closeSubject(row);
    else
        simulation_.openSubject(row);

    refreshAll();
}

void MainWindow::syncCourseEditorFromSelection()
{
    if (!courseTable_ || !tuitionSpin_ || !courseToggleButton_)
        return;

    const int row = courseTable_->currentRow();
    const auto& offers = simulation_.subjectOffers();

    if (row < 0 || row >= offers.size())
    {
        tuitionSpin_->setEnabled(false);
        courseToggleButton_->setEnabled(false);
        return;
    }

    const SubjectOffer& offer = offers.at(row);

    tuitionSpin_->setEnabled(true);
    tuitionSpin_->setRange(
        static_cast<int>(offer.minFee),
        static_cast<int>(offer.maxFee)
    );
    tuitionSpin_->setValue(static_cast<int>(offer.currentFee));

    courseToggleButton_->setText(
        offer.opened ? "과목 폐강" : "과목 개설"
    );

    if (offer.opened)
    {
        bool hasStudent = false;

        for (const Student& student : simulation_.students())
        {
            if (student.subject == offer.subject)
            {
                hasStudent = true;
                break;
            }
        }

        const bool specialty = offer.subject == simulation_.specialty();

        courseToggleButton_->setEnabled(!hasStudent && !specialty);
    }
    else
    {
        courseToggleButton_->setEnabled(simulation_.canOpenSubject(row));
    }
}

TrainingPlan MainWindow::selectedTrainingPlan() const
{
    if (!trainingCombo_)
        return TrainingPlan::Balanced;

    return static_cast<TrainingPlan>(
        trainingCombo_->currentData().toInt()
    );
}

void MainWindow::startClassWeek()
{
    if (weekResolutionPending_ || academyScene_->isAnimating())
        return;

    weekResolutionPending_ = true;

    gameTabs_->setCurrentIndex(0);
    gameTabs_->tabBar()->setEnabled(false);

    classStartButton_->setEnabled(false);
    classStartButton_->setText("수업 진행 중...");

    eventLabel_->setText(
        QString("%1 방식으로 이번 주 수업을 시작합니다.")
            .arg(simulation_.trainingPlanName(selectedTrainingPlan()))
    );

    academyScene_->startClassAnimation();
}

void MainWindow::finishClassWeek()
{
    if (!weekResolutionPending_)
        return;

    const WeekSummary summary =
        simulation_.advanceWeek(selectedTrainingPlan());

    weekResolutionPending_ = false;

    gameTabs_->tabBar()->setEnabled(true);
    classStartButton_->setEnabled(true);
    classStartButton_->setText("강의 시작 · 1주 진행");

    academyScene_->refreshScene();
    refreshAll();
    showWeeklyReport(summary);
}

void MainWindow::showWeeklyReport(const WeekSummary& summary)
{
    QDialog dialog(this);
    dialog.setWindowTitle("주간 운영 결과");
    dialog.resize(650, 540);

    auto* root = new QVBoxLayout(&dialog);

    auto* title = new QLabel("이번 주 운영 결과");
    title->setObjectName("pageTitle");

    auto* grid = new QGridLayout();
    grid->setSpacing(10);

    auto addResult = [grid](
        int row,
        int col,
        const QString& titleText,
        const QString& valueText
    )
    {
        auto* frame = new QFrame();
        frame->setObjectName("metricCard");

        auto* box = new QVBoxLayout(frame);
        box->setContentsMargins(13, 10, 13, 10);

        auto* title = new QLabel(titleText);
        title->setObjectName("metricTitle");

        auto* value = new QLabel(valueText);
        value->setObjectName("metricValue");

        box->addWidget(title);
        box->addWidget(value);

        grid->addWidget(frame, row, col);
    };

    addResult(
        0,
        0,
        "현금 변화",
        signedMoney(summary.cashAfter - summary.cashBefore)
    );

    addResult(
        0,
        1,
        "평판 변화",
        QString("%1").arg(summary.reputationAfter - summary.reputationBefore, 0, 10).prepend(
            summary.reputationAfter - summary.reputationBefore > 0 ? "+" : ""
        )
    );

    addResult(
        1,
        0,
        "학업능력 변화",
        signedNumber(summary.skillAfter - summary.skillBefore)
    );

    addResult(
        1,
        1,
        "만족도 변화",
        signedNumber(summary.satisfactionAfter - summary.satisfactionBefore)
    );

    addResult(
        2,
        0,
        "신규 상담",
        QString("+%1명").arg(summary.newApplicants)
    );

    addResult(
        2,
        1,
        "퇴원",
        QString("%1명").arg(summary.dropouts)
    );

    auto* eventTitle = new QLabel("이번 주 주요 변화");
    eventTitle->setObjectName("sectionTitle");

    auto* event = new QLabel(summary.eventText);
    event->setWordWrap(true);
    event->setObjectName("eventBox");

    root->addWidget(title);
    root->addLayout(grid);

    if (summary.examOccurred)
    {
        auto* exam = new QLabel(
            QString("%1 결과: 평균 %2점")
                .arg(summary.examName)
                .arg(summary.examAverage, 0, 'f', 1)
        );
        exam->setObjectName("assessmentBox");
        root->addWidget(exam);
    }

    root->addWidget(eventTitle);
    root->addWidget(event);

    auto* tipTitle = new QLabel("다음 주 팁");
    tipTitle->setObjectName("sectionTitle");

    auto* tip = new QLabel(summary.tip);
    tip->setWordWrap(true);

    root->addWidget(tipTitle);
    root->addWidget(tip);
    root->addStretch();

    auto* closeButton = new QPushButton("확인");

    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    root->addWidget(closeButton, 0, Qt::AlignRight);

    dialog.exec();
}

void MainWindow::refreshAll()
{
    academyTitleLabel_->setText(
        QString("%1 · %2 원장직강")
            .arg(simulation_.academyName(), simulation_.specialty())
    );

    dateLabel_->setText(simulation_.dateText());

    assessmentLabel_->setText(
        QString("다음 평가: %1").arg(simulation_.nextAssessmentText())
    );

    eventLabel_->setText(simulation_.latestEvent());

    cashLabel_->setText(money(simulation_.cash()));
    studentCountLabel_->setText(
        QString("%1명").arg(simulation_.students().size())
    );
    reputationLabel_->setText(QString::number(simulation_.reputation()));
    averageSkillLabel_->setText(
        QString::number(simulation_.averageScore(), 'f', 1)
    );

    if (simulation_.averageExamScore() > 0.0)
        averageExamLabel_->setText(
            QString::number(simulation_.averageExamScore(), 'f', 1)
        );
    else
        averageExamLabel_->setText("-");

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

    weeklyNetLabel_->setText(
        money(simulation_.lastFinance().net)
    );

    reputationBar_->setValue(simulation_.reputation());
    skillBar_->setValue(static_cast<int>(simulation_.averageScore()));
    satisfactionBar_->setValue(
        static_cast<int>(simulation_.averageSatisfaction())
    );
    stressBar_->setValue(
        static_cast<int>(simulation_.averageStress())
    );

    if (simulation_.teachers().isEmpty())
    {
        ownerStatusLabel_->setText(
            QString("강사 0명 · 원장이 %1만 직접 수업 중")
                .arg(simulation_.specialty())
        );
    }
    else
    {
        ownerStatusLabel_->setText(
            QString("강사 %1명 · 개설 과목은 과목·수강료 메뉴에서 관리")
                .arg(simulation_.teachers().size())
        );
    }

    refreshApplicants();
    refreshStudents();
    refreshTeachers();
    refreshCourses();
    refreshFacilities();
    refreshFinance();
    refreshRival();
    refreshTrainingDescription();

    academyScene_->refreshScene();
}

void MainWindow::refreshApplicants()
{
    const auto& applicants = simulation_.applicants();

    applicantTable_->setRowCount(
        static_cast<int>(applicants.size())
    );

    for (int row = 0; row < applicants.size(); ++row)
    {
        const Applicant& applicant = applicants.at(row);
        const Student& student = applicant.student;

        applicantTable_->setItem(row, 0, centeredItem(student.name));
        applicantTable_->setItem(row, 1, centeredItem(QString("%1학년").arg(student.schoolYear)));
        applicantTable_->setItem(row, 2, centeredItem(student.subject));
        applicantTable_->setItem(row, 3, centeredItem(money(student.monthlyTuition)));
        applicantTable_->setItem(row, 4, centeredItem(QString::number(student.score, 'f', 1)));
        applicantTable_->setItem(row, 5, centeredItem(QString::number(student.potential)));
        applicantTable_->setItem(row, 6, centeredItem(QString::number(student.diligence)));
        applicantTable_->setItem(row, 7, centeredItem(QString::number(student.concentration)));
        applicantTable_->setItem(row, 8, centeredItem(QString::number(student.stress, 'f', 0)));
        applicantTable_->setItem(row, 9, centeredItem(QString::number(applicant.consultationFit)));
        applicantTable_->setItem(row, 10, centeredItem(QString::number(applicant.parentDemand)));
        applicantTable_->setItem(row, 11, centeredItem(applicant.note));
    }
}

void MainWindow::refreshStudents()
{
    const auto& students = simulation_.students();

    studentTable_->setRowCount(
        static_cast<int>(students.size())
    );

    for (int row = 0; row < students.size(); ++row)
    {
        const Student& student = students.at(row);

        studentTable_->setItem(row, 0, centeredItem(student.name));
        studentTable_->setItem(row, 1, centeredItem(student.subject));
        studentTable_->setItem(row, 2, centeredItem(QString("%1학년").arg(student.schoolYear)));
        studentTable_->setItem(row, 3, centeredItem(QString::number(student.score, 'f', 1)));
        studentTable_->setItem(
            row,
            4,
            centeredItem(
                student.examCount > 0
                    ? QString::number(student.lastExamScore, 'f', 1)
                    : "-"
            )
        );
        studentTable_->setItem(row, 5, centeredItem(QString::number(student.potential)));
        studentTable_->setItem(row, 6, centeredItem(QString::number(student.diligence)));
        studentTable_->setItem(row, 7, centeredItem(QString::number(student.comprehension)));
        studentTable_->setItem(row, 8, centeredItem(QString::number(student.memorization)));
        studentTable_->setItem(row, 9, centeredItem(QString::number(student.concentration)));
        studentTable_->setItem(row, 10, centeredItem(QString::number(student.examNerves)));
        studentTable_->setItem(row, 11, centeredItem(QString::number(student.stress, 'f', 1)));
        studentTable_->setItem(row, 12, centeredItem(QString::number(student.fatigue, 'f', 1)));
        studentTable_->setItem(row, 13, centeredItem(QString::number(student.satisfaction, 'f', 1)));
        studentTable_->setItem(row, 14, centeredItem(student.conditionText()));
    }
}

void MainWindow::refreshTeachers()
{
    const auto& teachers = simulation_.teachers();

    teacherTable_->setRowCount(
        static_cast<int>(teachers.size())
    );

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
        teacherTable_->setItem(row, 8, centeredItem(money(teacher.monthlySalary)));
    }

    const auto& candidates = simulation_.teacherCandidates();

    teacherCandidateTable_->setRowCount(
        static_cast<int>(candidates.size())
    );

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

void MainWindow::refreshCourses()
{
    const auto& offers = simulation_.subjectOffers();

    courseTable_->setRowCount(
        static_cast<int>(offers.size())
    );

    for (int row = 0; row < offers.size(); ++row)
    {
        const SubjectOffer& offer = offers.at(row);

        const double ratio =
            static_cast<double>(offer.currentFee)
            / static_cast<double>(offer.standardFee);

        QString priceEffect = "기준";

        if (ratio <= 0.85)
            priceEffect = "상담 크게 증가 / 수익 감소";
        else if (ratio < 0.98)
            priceEffect = "상담 증가";
        else if (ratio > 1.10)
            priceEffect = "상담 감소 / 평판·퇴원 위험";
        else if (ratio > 1.02)
            priceEffect = "매출 증가 / 상담 소폭 감소";

        QString teacherState;

        if (offer.subject == simulation_.specialty())
            teacherState = "원장";
        else if (simulation_.hasTeacherForSubject(offer.subject))
            teacherState = "강사 확보";
        else
            teacherState = "담당 없음";

        courseTable_->setItem(row, 0, centeredItem(offer.subject));
        courseTable_->setItem(row, 1, centeredItem(offer.opened ? "개설" : "미개설"));
        courseTable_->setItem(row, 2, centeredItem(teacherState));
        courseTable_->setItem(row, 3, centeredItem(money(offer.standardFee)));
        courseTable_->setItem(row, 4, centeredItem(money(offer.currentFee)));
        courseTable_->setItem(
            row,
            5,
            centeredItem(
                QString("%1 ~ %2")
                    .arg(money(offer.minFee))
                    .arg(money(offer.maxFee))
            )
        );
        courseTable_->setItem(row, 6, centeredItem(priceEffect));
    }

    if (courseTable_->currentRow() < 0 && !offers.isEmpty())
        courseTable_->selectRow(0);

    syncCourseEditorFromSelection();
}

void MainWindow::refreshFacilities()
{
    const FacilityState& f = simulation_.facilities();

    roomUsageLabel_->setText(
        QString("방 사용: %1 / %2칸 · 학생 정원: %3명. 방 슬롯이 부족하면 먼저 학원 면적을 넓혀야 합니다.")
            .arg(f.usedRoomSlots())
            .arg(f.roomSlots())
            .arg(f.capacity())
    );

    floorLabel_->setText(
        QString("확장 Lv.%1 · 총 방 %2칸\n다음 비용 %3")
            .arg(f.floorExpansionLevel)
            .arg(f.roomSlots())
            .arg(money(simulation_.floorExpansionCost()))
    );

    classroomLabel_->setText(
        QString("%1개 · 정원 %2명\n추가 비용 %3")
            .arg(f.classrooms)
            .arg(f.capacity())
            .arg(money(simulation_.classroomCost()))
    );

    directorOfficeLabel_->setText(
        f.directorOffice
            ? "완공 · 운영 체계 및 평판 보조"
            : QString("미설치\n공사비 %1").arg(money(simulation_.directorOfficeCost()))
    );

    consultationRoomLabel_->setText(
        f.consultationRoom
            ? "완공 · 상담 유입과 상담 적합도 증가"
            : QString("미설치\n공사비 %1").arg(money(simulation_.consultationRoomCost()))
    );

    pantryLabel_->setText(
        f.pantry
            ? "완공 · 피로/스트레스 관리 보조"
            : QString("미설치\n공사비 %1").arg(money(simulation_.pantryCost()))
    );

    studyRoomLabel_->setText(
        QString("%1개 · 학업 성장 및 컨디션 보조\n다음 비용 %2")
            .arg(f.studyRooms)
            .arg(money(simulation_.studyRoomCost()))
    );

    interiorLabel_->setText(
        QString("Lv.%1 · 만족도/상담 인상 개선\n다음 비용 %2")
            .arg(f.interiorLevel)
            .arg(money(simulation_.interiorUpgradeCost()))
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

    const int reputationGap =
        simulation_.reputation() - rival.reputation;

    const int studentGap =
        static_cast<int>(simulation_.students().size())
        - rival.students;

    QString summary;

    if (reputationGap >= 8)
        summary = "평판 경쟁에서는 우리 학원이 앞서고 있습니다.";
    else if (reputationGap <= -8)
        summary = "평판에서 경쟁 학원에 밀리고 있습니다.";
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
            "학업능력 상승 효율이 가장 높습니다. 대신 스트레스와 피로가 빠르게 쌓이고 퇴원 위험이 커질 수 있습니다."
        );
        break;
    case TrainingPlan::Fundamentals:
        trainingDescriptionLabel_->setText(
            "성장 속도는 조금 느리지만 안정적인 누적 성장을 노립니다. 시험 직전보다 장기 육성에 적합합니다."
        );
        break;
    case TrainingPlan::Care:
        trainingDescriptionLabel_->setText(
            "성장 속도를 일부 포기하고 스트레스와 피로를 낮추며 만족도를 회복합니다."
        );
        break;
    case TrainingPlan::Balanced:
    default:
        trainingDescriptionLabel_->setText(
            "학업 성장과 컨디션을 함께 보는 기본 운영입니다. 특별한 문제가 없을 때 가장 안정적입니다."
        );
        break;
    }
}

QString MainWindow::defaultIntroText() const
{
    return
        "몇 년 동안 남의 학원에서 강의하고, 상담하고, 학생을 붙잡아 왔다.\n\n"
        "이제는 내 이름으로 된 학원을 열기로 했다.\n"
        "처음부터 모든 과목을 가르칠 수는 없다. 내가 직접 맡을 단과 하나와 작은 강의실 하나뿐이다.\n"
        "좋은 학생을 만나는 것도 운이고, 좋은 강사를 데려오는 것도 돈이다.\n"
        "학원비를 낮추면 상담은 늘겠지만 남는 돈이 줄고, 올리면 당장 매출은 늘어도 학생과 동네의 시선이 달라진다.\n"
        "시설을 넓히고 과목을 늘리면 더 큰 학원이 되겠지만 임대료와 공과금도 따라온다.\n\n"
        "처음 가진 것은 3,500만원과 내가 가르칠 수 있는 한 과목.\n"
        "그리고 바로 옆에는 이미 자리를 잡은 경쟁 학원이 있다.\n\n"
        "어떤 학생을 받을 것인가.\n"
        "누구를 강사로 고용할 것인가.\n"
        "이번 주에는 학생을 얼마나 밀어붙일 것인가.\n\n"
        "학원 운영을 시작한다.";
}

QString MainWindow::money(qint64 value)
{
    const bool negative = value < 0;

    const quint64 absolute =
        negative
            ? static_cast<quint64>(-value)
            : static_cast<quint64>(value);

    QString digits = QString::number(absolute);

    for (int i = digits.size() - 3; i > 0; i -= 3)
        digits.insert(i, ',');

    return QString("%1₩%2")
        .arg(negative ? "-" : "")
        .arg(digits);
}

QString MainWindow::signedNumber(double value, int decimals)
{
    return QString("%1%2")
        .arg(value > 0.0 ? "+" : "")
        .arg(QString::number(value, 'f', decimals));
}

QString MainWindow::signedMoney(qint64 value)
{
    if (value > 0)
        return "+" + money(value);

    return money(value);
}

QTableWidgetItem* MainWindow::centeredItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}
