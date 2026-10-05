#include "MainWindow.h"

#include <QAbstractItemView>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
QTableWidgetItem* cell(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("학원키우기 v0.1");
    resize(1240, 780);
    setMinimumSize(1040, 680);

    auto* central = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(20, 18, 20, 20);
    rootLayout->setSpacing(14);

    auto* topBar = new QHBoxLayout();

    auto* title = new QLabel("학원키우기");
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);

    dateLabel_ = new QLabel();
    dateLabel_->setObjectName("dateLabel");

    auto* weekButton = new QPushButton("1주 진행");
    auto* monthButton = new QPushButton("4주 진행");
    weekButton->setMinimumHeight(38);
    monthButton->setMinimumHeight(38);

    connect(weekButton, &QPushButton::clicked, this, [this] {
        advanceWeeks(1);
    });
    connect(monthButton, &QPushButton::clicked, this, [this] {
        advanceWeeks(4);
    });

    topBar->addWidget(title);
    topBar->addSpacing(18);
    topBar->addWidget(dateLabel_);
    topBar->addStretch();
    topBar->addWidget(weekButton);
    topBar->addWidget(monthButton);

    auto* tabs = new QTabWidget();
    tabs->addTab(createDashboardTab(), "대시보드");
    tabs->addTab(createStudentsTab(), "학생");
    tabs->addTab(createTeachersTab(), "강사");
    tabs->addTab(createFinanceTab(), "재정");

    rootLayout->addLayout(topBar);
    rootLayout->addWidget(tabs, 1);

    setCentralWidget(central);

    setStyleSheet(R"(
        QMainWindow, QWidget {
            background: #f4f6f8;
            color: #20252b;
            font-size: 14px;
        }

        QTabWidget::pane {
            border: 1px solid #d9dee5;
            background: white;
            border-radius: 8px;
        }

        QTabBar::tab {
            background: #e8ecf1;
            border: none;
            padding: 11px 22px;
            margin-right: 2px;
        }

        QTabBar::tab:selected {
            background: #ffffff;
            font-weight: 700;
        }

        QPushButton {
            background: #253858;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 15px;
            font-weight: 700;
        }

        QPushButton:hover {
            background: #334e78;
        }

        QFrame#metricCard {
            background: white;
            border: 1px solid #dde2e8;
            border-radius: 10px;
        }

        QLabel#metricTitle {
            color: #66717f;
            font-size: 12px;
        }

        QLabel#metricValue {
            color: #172b4d;
            font-size: 23px;
            font-weight: 800;
        }

        QLabel#sectionTitle {
            color: #172b4d;
            font-size: 16px;
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

    refreshAll();
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
    layout->setSpacing(18);

    auto* metrics = new QGridLayout();
    metrics->setSpacing(12);

    metrics->addWidget(createMetricCard("보유 자금", cashLabel_), 0, 0);
    metrics->addWidget(createMetricCard("재학생", studentCountLabel_), 0, 1);
    metrics->addWidget(createMetricCard("학원 평판", reputationLabel_), 0, 2);
    metrics->addWidget(createMetricCard("평균 성적", averageScoreLabel_), 0, 3);

    layout->addLayout(metrics);

    auto* sectionTitle = new QLabel("학원 상태");
    sectionTitle->setObjectName("sectionTitle");
    layout->addWidget(sectionTitle);

    auto* statusFrame = new QFrame();
    statusFrame->setObjectName("metricCard");
    auto* statusLayout = new QGridLayout(statusFrame);
    statusLayout->setContentsMargins(18, 18, 18, 18);
    statusLayout->setHorizontalSpacing(18);
    statusLayout->setVerticalSpacing(14);

    reputationBar_ = new QProgressBar();
    reputationBar_->setRange(0, 100);

    scoreBar_ = new QProgressBar();
    scoreBar_->setRange(0, 100);

    satisfactionBar_ = new QProgressBar();
    satisfactionBar_->setRange(0, 100);

    averageSatisfactionLabel_ = new QLabel();
    weeklyNetLabel_ = new QLabel();

    statusLayout->addWidget(new QLabel("평판"), 0, 0);
    statusLayout->addWidget(reputationBar_, 0, 1);
    statusLayout->addWidget(new QLabel("평균 성적"), 1, 0);
    statusLayout->addWidget(scoreBar_, 1, 1);
    statusLayout->addWidget(new QLabel("학생 만족도"), 2, 0);
    statusLayout->addWidget(satisfactionBar_, 2, 1);
    statusLayout->addWidget(new QLabel("평균 만족도"), 3, 0);
    statusLayout->addWidget(averageSatisfactionLabel_, 3, 1);
    statusLayout->addWidget(new QLabel("이번 주 손익"), 4, 0);
    statusLayout->addWidget(weeklyNetLabel_, 4, 1);

    layout->addWidget(statusFrame);
    layout->addStretch();

    return page;
}

QWidget* MainWindow::createStudentsTab()
{
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);

    auto* title = new QLabel("재학생 명단");
    title->setObjectName("sectionTitle");

    studentTable_ = new QTableWidget();
    studentTable_->setColumnCount(7);
    studentTable_->setHorizontalHeaderLabels({
        "이름", "학년", "현재 성적", "잠재력", "성실성", "스트레스", "만족도"
    });
    studentTable_->setAlternatingRowColors(true);
    studentTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    studentTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    studentTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

    auto* title = new QLabel("강사진");
    title->setObjectName("sectionTitle");

    teacherTable_ = new QTableWidget();
    teacherTable_->setColumnCount(5);
    teacherTable_->setHorizontalHeaderLabels({
        "이름", "과목", "강의력", "학생관리", "월 급여"
    });
    teacherTable_->setAlternatingRowColors(true);
    teacherTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    teacherTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    teacherTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    teacherTable_->verticalHeader()->setVisible(false);

    layout->addWidget(title);
    layout->addWidget(teacherTable_, 1);

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
    panel->setObjectName("metricCard");
    auto* grid = new QGridLayout(panel);
    grid->setContentsMargins(20, 20, 20, 20);
    grid->setVerticalSpacing(16);

    revenueLabel_ = new QLabel();
    payrollLabel_ = new QLabel();
    rentLabel_ = new QLabel();
    financeNetLabel_ = new QLabel();

    grid->addWidget(new QLabel("수강료 매출"), 0, 0);
    grid->addWidget(revenueLabel_, 0, 1);
    grid->addWidget(new QLabel("강사 급여"), 1, 0);
    grid->addWidget(payrollLabel_, 1, 1);
    grid->addWidget(new QLabel("임대료/고정비"), 2, 0);
    grid->addWidget(rentLabel_, 2, 1);
    grid->addWidget(new QLabel("주간 순손익"), 3, 0);
    grid->addWidget(financeNetLabel_, 3, 1);

    layout->addWidget(title);
    layout->addWidget(panel);
    layout->addStretch();

    return page;
}

void MainWindow::advanceWeeks(int count)
{
    for (int i = 0; i < count; ++i)
        simulation_.advanceWeek();

    refreshAll();
}

void MainWindow::refreshAll()
{
    dateLabel_->setText(simulation_.dateText());
    cashLabel_->setText(money(simulation_.cash()));
    studentCountLabel_->setText(QString("%1명").arg(simulation_.students().size()));
    reputationLabel_->setText(QString::number(simulation_.reputation()));
    averageScoreLabel_->setText(QString::number(simulation_.averageScore(), 'f', 1));

    averageSatisfactionLabel_->setText(
        QString::number(simulation_.averageSatisfaction(), 'f', 1)
    );

    reputationBar_->setValue(simulation_.reputation());
    scoreBar_->setValue(static_cast<int>(simulation_.averageScore()));
    satisfactionBar_->setValue(
        static_cast<int>(simulation_.averageSatisfaction())
    );

    refreshStudents();
    refreshTeachers();
    refreshFinance();
}

void MainWindow::refreshStudents()
{
    const auto& students = simulation_.students();
    studentTable_->setRowCount(static_cast<int>(students.size()));

    for (int row = 0; row < students.size(); ++row)
    {
        const auto& student = students.at(row);

        studentTable_->setItem(row, 0, cell(student.name));
        studentTable_->setItem(row, 1, cell(QString("%1학년").arg(student.schoolYear)));
        studentTable_->setItem(row, 2, cell(QString::number(student.score, 'f', 1)));
        studentTable_->setItem(row, 3, cell(QString::number(student.potential)));
        studentTable_->setItem(row, 4, cell(QString::number(student.diligence)));
        studentTable_->setItem(row, 5, cell(QString::number(student.stress, 'f', 1)));
        studentTable_->setItem(row, 6, cell(QString::number(student.satisfaction, 'f', 1)));
    }
}

void MainWindow::refreshTeachers()
{
    const auto& teachers = simulation_.teachers();
    teacherTable_->setRowCount(static_cast<int>(teachers.size()));

    for (int row = 0; row < teachers.size(); ++row)
    {
        const auto& teacher = teachers.at(row);

        teacherTable_->setItem(row, 0, cell(teacher.name));
        teacherTable_->setItem(row, 1, cell(teacher.subject));
        teacherTable_->setItem(row, 2, cell(QString::number(teacher.teaching)));
        teacherTable_->setItem(row, 3, cell(QString::number(teacher.management)));
        teacherTable_->setItem(row, 4, cell(money(teacher.monthlySalary)));
    }
}

void MainWindow::refreshFinance()
{
    const auto& finance = simulation_.lastFinance();

    revenueLabel_->setText(money(finance.revenue));
    payrollLabel_->setText("- " + money(finance.payroll));
    rentLabel_->setText("- " + money(finance.rent));
    weeklyNetLabel_->setText(money(finance.net));
    financeNetLabel_->setText(money(finance.net));
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
