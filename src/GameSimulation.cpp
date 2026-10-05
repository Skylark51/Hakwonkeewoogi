#include "GameSimulation.h"

#include <QRandomGenerator>
#include <QtGlobal>

#include <cmath>

namespace
{
double clampValue(double value, double low, double high)
{
    return qBound(low, value, high);
}
}

QString Student::conditionText() const
{
    if (stress >= 80.0 || fatigue >= 80.0)
        return "위험";
    if (stress >= 65.0 || fatigue >= 65.0)
        return "주의";
    if (satisfaction < 45.0)
        return "불만";
    if (attendance < 75.0)
        return "결석 잦음";
    return "양호";
}

GameSimulation::GameSimulation()
{
    initializeSubjectOffers();
    rival_.specialty = "영어";
}

void GameSimulation::initializeSubjectOffers()
{
    subjectOffers_.clear();

    subjectOffers_ = {
        {"수학", 400000, 400000, 240000, 480000, false},
        {"영어", 400000, 400000, 240000, 480000, false},
        {"국어", 300000, 300000, 180000, 360000, false},
        {"과학", 250000, 250000, 150000, 300000, false},
        {"사회", 250000, 250000, 150000, 300000, false}
    };
}

void GameSimulation::setupAcademy(const QString& academyName, const QString& specialty)
{
    academyName_ = academyName.trimmed().isEmpty() ? "새싹 학원" : academyName.trimmed();
    specialty_ = specialty;

    students_.clear();
    applicants_.clear();
    teachers_.clear();
    teacherCandidates_.clear();

    initializeSubjectOffers();

    for (SubjectOffer& offer : subjectOffers_)
        offer.opened = (offer.subject == specialty_);

    facilities_ = FacilityState{};
    lastFinance_ = FinanceSnapshot{};
    lastWeekSummary_ = WeekSummary{};

    week_ = 1;
    reputation_ = 35;
    cash_ = 35000000;
    lastExamAverage_ = 0.0;

    rival_.name = "탑클래스 학원";
    rival_.specialty = specialty_ == "수학" ? "영어" : "수학";
    rival_.reputation = 48;
    rival_.students = 42;
    rival_.monthlyTuition = 450000;

    // 첫 상담 풀에는 반드시 잠재력 높은 학생 1명을 포함한다.
    generateApplicants(5, true);
    generateTeacherCandidates(5);

    latestEvent_ =
        QString("%1이(가) 개원했습니다. 현재 개설 과목은 %2뿐이며 원장이 직접 수업합니다.")
            .arg(academyName_, specialty_);
}

int GameSimulation::randomInt(int lowInclusive, int highInclusive) const
{
    return lowInclusive
        + QRandomGenerator::global()->bounded(highInclusive - lowInclusive + 1);
}

int GameSimulation::randomPotential(bool forceHigh) const
{
    if (forceHigh)
        return randomInt(90, 97);

    const int roll = randomInt(1, 100);

    if (roll <= 3)
        return randomInt(90, 97);
    if (roll <= 14)
        return randomInt(80, 89);
    if (roll <= 47)
        return randomInt(68, 79);

    return randomInt(50, 67);
}

QString GameSimulation::randomOpenedSubject() const
{
    QVector<QString> opened;

    for (const SubjectOffer& offer : subjectOffers_)
    {
        if (offer.opened)
            opened.push_back(offer.subject);
    }

    if (opened.isEmpty())
        return specialty_;

    return opened.at(randomInt(0, static_cast<int>(opened.size()) - 1));
}

void GameSimulation::generateApplicants(int count, bool guaranteeHighPotential)
{
    static const QStringList surnames = {
        "김", "이", "박", "최", "정", "강", "조", "윤", "장", "임", "한"
    };
    static const QStringList givenNames = {
        "민준", "서준", "도윤", "지호", "예준", "서연", "지우", "하윤",
        "민서", "지민", "수빈", "현우", "유진", "채원", "시우", "예은",
        "준영", "서현", "은우", "나연", "태윤", "다은"
    };

    for (int i = 0; i < count; ++i)
    {
        Applicant applicant;
        Student& student = applicant.student;

        student.name =
            surnames.at(randomInt(0, static_cast<int>(surnames.size()) - 1))
            + givenNames.at(randomInt(0, static_cast<int>(givenNames.size()) - 1));

        student.schoolYear = randomInt(1, 3);
        student.subject = randomOpenedSubject();

        const bool forceHigh = guaranteeHighPotential && i == 0;

        student.score = randomInt(42, 84);
        student.potential = randomPotential(forceHigh);
        student.diligence = randomInt(35, 95);
        student.comprehension = randomInt(40, 96);
        student.memorization = randomInt(40, 96);
        student.concentration = randomInt(35, 95);
        student.examNerves = randomInt(30, 95);

        student.stress = randomInt(10, 62);
        student.fatigue = randomInt(8, 50);
        student.satisfaction = randomInt(55, 85);
        student.attendance = randomInt(80, 100);
        student.monthlyTuition = tuitionForSubject(student.subject);

        const double priceRatio = tuitionRatioForSubject(student.subject);
        const int priceFitAdjustment =
            static_cast<int>((1.0 - priceRatio) * 35.0);

        applicant.consultationFit = qBound(
            25,
            randomInt(45, 92)
                + priceFitAdjustment
                + (facilities_.consultationRoom ? 7 : 0),
            100
        );

        applicant.parentDemand = randomInt(20, 95);

        if (forceHigh)
            applicant.note = "개원 초기 특별 상담 · 성장 잠재력이 매우 높음";
        else if (student.potential >= 90)
            applicant.note = "희귀한 상위 잠재력";
        else if (student.potential >= 82)
            applicant.note = "성장 잠재력이 높음";
        else if (applicant.parentDemand >= 82)
            applicant.note = "학부모 요구 수준이 높음";
        else if (student.stress >= 55)
            applicant.note = "현재 학업 스트레스가 높은 편";
        else if (student.diligence >= 82)
            applicant.note = "과제 수행이 성실함";
        else
            applicant.note = "특이사항 없음";

        applicants_.push_back(applicant);
    }
}

int GameSimulation::generateWeeklyApplicants()
{
    if (applicants_.size() >= 14)
        return 0;

    double demand = 0.0;
    int openedCount = 0;

    for (const SubjectOffer& offer : subjectOffers_)
    {
        if (!offer.opened)
            continue;

        ++openedCount;

        const double ratio =
            static_cast<double>(offer.currentFee)
            / static_cast<double>(offer.standardFee);

        const double priceModifier =
            qBound(0.45, 1.0 - (ratio - 1.0) * 1.8, 1.60);

        demand += 0.85 * priceModifier;
    }

    demand += reputation_ / 55.0;

    if (facilities_.consultationRoom)
        demand += 0.7;

    if (facilities_.interiorLevel >= 3)
        demand += 0.35;

    if (openedCount >= 3)
        demand += 0.5;

    int count = qBound(
        0,
        static_cast<int>(std::round(demand + randomInt(-1, 1))),
        6
    );

    count = qMin(count, 14 - static_cast<int>(applicants_.size()));

    if (count > 0)
        generateApplicants(count, false);

    return count;
}

void GameSimulation::generateTeacherCandidates(int targetCount)
{
    static const QStringList surnames = {
        "김", "이", "박", "최", "정", "오", "송", "백", "문", "신"
    };
    static const QStringList givenNames = {
        "도현", "서윤", "준호", "민재", "유나", "현석", "지혜", "성민",
        "가영", "승현", "태훈", "지은", "예린", "동현", "수진"
    };
    static const QStringList subjects = {"수학", "영어", "국어", "과학", "사회"};
    static const QStringList traits = {
        "스타강사형", "엄격한 관리형", "친근한 상담형",
        "연구·교재형", "체력 좋은 열정형", "무난한 균형형"
    };

    while (teacherCandidates_.size() < targetCount)
    {
        Teacher teacher;

        teacher.name =
            surnames.at(randomInt(0, static_cast<int>(surnames.size()) - 1))
            + givenNames.at(randomInt(0, static_cast<int>(givenNames.size()) - 1));

        teacher.subject = randomInt(1, 100) <= 35
            ? specialty_
            : subjects.at(randomInt(0, static_cast<int>(subjects.size()) - 1));

        teacher.teaching = randomInt(52, 92);
        teacher.counseling = randomInt(40, 92);
        teacher.discipline = randomInt(40, 92);
        teacher.charisma = randomInt(35, 95);
        teacher.stamina = randomInt(45, 95);

        teacher.trait = traits.at(randomInt(0, static_cast<int>(traits.size()) - 1));

        if (teacher.trait == "스타강사형")
        {
            teacher.charisma = qMax(teacher.charisma, 86);
            teacher.monthlySalary = 4800000 + randomInt(0, 10) * 100000;
        }
        else if (teacher.trait == "엄격한 관리형")
        {
            teacher.discipline = qMax(teacher.discipline, 84);
            teacher.monthlySalary = 3700000 + randomInt(0, 8) * 100000;
        }
        else if (teacher.trait == "친근한 상담형")
        {
            teacher.counseling = qMax(teacher.counseling, 86);
            teacher.monthlySalary = 3600000 + randomInt(0, 8) * 100000;
        }
        else
        {
            teacher.monthlySalary = 3300000 + randomInt(0, 12) * 100000;
        }

        teacher.signingCost = teacher.monthlySalary / 3;
        teacherCandidates_.push_back(teacher);
    }
}

bool GameSimulation::acceptApplicant(int index)
{
    if (index < 0 || index >= applicants_.size())
        return false;

    if (students_.size() >= facilities_.capacity())
    {
        latestEvent_ = "정원이 가득 찼습니다. 강의실을 늘려야 신규 학생을 받을 수 있습니다.";
        return false;
    }

    const Applicant applicant = applicants_.at(index);

    bool subjectOpened = false;
    for (const SubjectOffer& offer : subjectOffers_)
    {
        if (offer.subject == applicant.student.subject && offer.opened)
        {
            subjectOpened = true;
            break;
        }
    }

    if (!subjectOpened)
    {
        latestEvent_ = "현재 개설되지 않은 과목의 학생입니다.";
        return false;
    }

    students_.push_back(applicant.student);
    applicants_.removeAt(index);

    latestEvent_ =
        QString("상담 결과 %1 학생을 등록시켰습니다. %2 수강을 시작합니다.")
            .arg(applicant.student.name, applicant.student.subject);

    return true;
}

bool GameSimulation::rejectApplicant(int index)
{
    if (index < 0 || index >= applicants_.size())
        return false;

    const QString name = applicants_.at(index).student.name;
    applicants_.removeAt(index);

    latestEvent_ = QString("%1 학생의 등록을 받지 않기로 결정했습니다.").arg(name);
    return true;
}

bool GameSimulation::hireTeacher(int index)
{
    if (index < 0 || index >= teacherCandidates_.size())
        return false;

    const Teacher candidate = teacherCandidates_.at(index);

    if (cash_ < candidate.signingCost)
    {
        latestEvent_ = "채용 계약금을 지불할 자금이 부족합니다.";
        return false;
    }

    cash_ -= candidate.signingCost;
    teachers_.push_back(candidate);
    teacherCandidates_.removeAt(index);

    latestEvent_ =
        QString("%1 %2 강사를 채용했습니다. 이제 운영 메뉴에서 %2 과목을 개설할 수 있습니다. 특징: %3")
            .arg(candidate.name, candidate.subject, candidate.trait);

    generateTeacherCandidates(5);
    return true;
}

bool GameSimulation::hasTeacherForSubject(const QString& subject) const
{
    if (subject == specialty_)
        return true;

    for (const Teacher& teacher : teachers_)
    {
        if (teacher.subject == subject)
            return true;
    }

    return false;
}

bool GameSimulation::canOpenSubject(int subjectIndex) const
{
    if (subjectIndex < 0 || subjectIndex >= subjectOffers_.size())
        return false;

    const SubjectOffer& offer = subjectOffers_.at(subjectIndex);
    return !offer.opened && hasTeacherForSubject(offer.subject);
}

bool GameSimulation::openSubject(int subjectIndex)
{
    if (!canOpenSubject(subjectIndex))
    {
        latestEvent_ = "해당 과목을 담당할 강사가 없어 개설할 수 없습니다.";
        return false;
    }

    SubjectOffer& offer = subjectOffers_[subjectIndex];
    offer.opened = true;

    latestEvent_ =
        QString("%1 과목을 개설했습니다. 다음 주부터 %1 상담 문의가 들어올 수 있습니다.")
            .arg(offer.subject);

    return true;
}

bool GameSimulation::closeSubject(int subjectIndex)
{
    if (subjectIndex < 0 || subjectIndex >= subjectOffers_.size())
        return false;

    SubjectOffer& offer = subjectOffers_[subjectIndex];

    if (offer.subject == specialty_)
    {
        latestEvent_ = "원장의 주력 과목은 현재 단계에서 폐강할 수 없습니다.";
        return false;
    }

    for (const Student& student : students_)
    {
        if (student.subject == offer.subject)
        {
            latestEvent_ = "현재 수강 중인 학생이 있어 해당 과목을 폐강할 수 없습니다.";
            return false;
        }
    }

    offer.opened = false;
    latestEvent_ = QString("%1 과목을 폐강했습니다.").arg(offer.subject);
    return true;
}

bool GameSimulation::setTuition(int subjectIndex, qint64 monthlyFee)
{
    if (subjectIndex < 0 || subjectIndex >= subjectOffers_.size())
        return false;

    SubjectOffer& offer = subjectOffers_[subjectIndex];
    const qint64 clampedFee = qBound(offer.minFee, monthlyFee, offer.maxFee);

    offer.currentFee = clampedFee;

    for (Student& student : students_)
    {
        if (student.subject == offer.subject)
            student.monthlyTuition = clampedFee;
    }

    const double ratio =
        static_cast<double>(offer.currentFee)
        / static_cast<double>(offer.standardFee);

    if (ratio > 1.08)
    {
        latestEvent_ =
            QString("%1 월 수강료를 %2원으로 조정했습니다. 기준가보다 높아 상담 수요와 평판에 부담이 생길 수 있습니다.")
                .arg(offer.subject)
                .arg(offer.currentFee);
    }
    else if (ratio < 0.90)
    {
        latestEvent_ =
            QString("%1 월 수강료를 %2원으로 낮췄습니다. 상담 문의는 늘 수 있지만 학생당 매출이 감소합니다.")
                .arg(offer.subject)
                .arg(offer.currentFee);
    }
    else
    {
        latestEvent_ =
            QString("%1 월 수강료를 %2원으로 조정했습니다.")
                .arg(offer.subject)
                .arg(offer.currentFee);
    }

    return true;
}

double GameSimulation::teacherEffectFor(const QString& subject) const
{
    double best = subject == specialty_ ? 64.0 : 0.0;

    for (const Teacher& teacher : teachers_)
    {
        if (teacher.subject != subject)
            continue;

        const double value =
            teacher.teaching * 0.68
            + teacher.discipline * 0.14
            + teacher.counseling * 0.10
            + teacher.stamina * 0.08;

        best = qMax(best, value);
    }

    return best;
}

WeekSummary GameSimulation::advanceWeek(TrainingPlan plan)
{
    WeekSummary summary;

    summary.cashBefore = cash_;
    summary.reputationBefore = reputation_;
    summary.skillBefore = averageScore();
    summary.satisfactionBefore = averageSatisfaction();

    updateStudents(plan);
    runScheduledExam(summary);

    summary.dropouts = processDropouts();

    updateFinance();
    updateReputation();
    updateRival();

    summary.newApplicants = generateWeeklyApplicants();

    resolveWeeklyEvent();

    if (summary.examOccurred)
    {
        latestEvent_ =
            QString("%1 실시 · 평균 %2점. %3")
                .arg(summary.examName)
                .arg(summary.examAverage, 0, 'f', 1)
                .arg(latestEvent_);
    }

    if (summary.dropouts > 0)
    {
        latestEvent_ +=
            QString(" 이번 주 %1명이 퇴원했습니다.").arg(summary.dropouts);
    }

    summary.cashAfter = cash_;
    summary.reputationAfter = reputation_;
    summary.skillAfter = averageScore();
    summary.satisfactionAfter = averageSatisfaction();
    summary.eventText = latestEvent_;
    summary.tip = weeklyTip();

    lastWeekSummary_ = summary;

    ++week_;

    if (teacherCandidates_.size() < 5)
        generateTeacherCandidates(5);

    return summary;
}

void GameSimulation::updateStudents(TrainingPlan plan)
{
    for (Student& student : students_)
    {
        const double teacherEffect = teacherEffectFor(student.subject);

        if (teacherEffect <= 0.0)
            continue;

        const double talentFactor =
            student.potential * 0.004
            + student.comprehension * 0.004
            + student.memorization * 0.002;

        const double disciplineFactor =
            student.diligence * 0.004
            + student.concentration * 0.003
            + student.attendance * 0.002;

        double growthMultiplier = 1.0;
        double stressDelta = 0.0;
        double fatigueDelta = 0.0;
        double satisfactionDelta = 0.0;

        switch (plan)
        {
        case TrainingPlan::ScoreIntensive:
            growthMultiplier = 1.24;
            stressDelta = 5.0;
            fatigueDelta = 5.5;
            satisfactionDelta = -1.6;
            break;
        case TrainingPlan::Fundamentals:
            growthMultiplier = 0.94;
            stressDelta = 1.0;
            fatigueDelta = 1.8;
            satisfactionDelta = 0.8;
            break;
        case TrainingPlan::Care:
            growthMultiplier = 0.76;
            stressDelta = -5.5;
            fatigueDelta = -4.0;
            satisfactionDelta = 3.5;
            break;
        case TrainingPlan::Balanced:
        default:
            growthMultiplier = 1.0;
            stressDelta = 2.2;
            fatigueDelta = 2.5;
            satisfactionDelta = 0.5;
            break;
        }

        const double facilityGrowthBonus =
            facilities_.studyRooms * 0.07
            + facilities_.interiorLevel * 0.015;

        const double conditionPenalty =
            student.stress * 0.0035 + student.fatigue * 0.003;

        const double randomFactor = randomInt(-12, 12) / 100.0;

        double growth =
            (
                0.12
                + teacherEffect * 0.010
                + talentFactor
                + disciplineFactor
                + facilityGrowthBonus
                - conditionPenalty
                + randomFactor
            ) * growthMultiplier;

        if (student.score > student.potential)
            growth *= 0.55;

        if (student.score >= 95.0)
            growth *= 0.30;

        student.score = clampValue(student.score + growth, 0.0, 100.0);

        double relief = 0.0;

        if (facilities_.pantry)
            relief += 0.8;

        if (facilities_.studyRooms > 0)
            relief += facilities_.studyRooms * 0.4;

        student.stress = clampValue(
            student.stress + stressDelta - relief + randomInt(-2, 2),
            0.0,
            100.0
        );

        student.fatigue = clampValue(
            student.fatigue + fatigueDelta - (facilities_.pantry ? 0.6 : 0.0)
                + randomInt(-2, 2),
            0.0,
            100.0
        );

        student.satisfaction = clampValue(
            student.satisfaction
                + satisfactionDelta
                + growth * 0.35
                + facilities_.interiorLevel * 0.10
                + (facilities_.pantry ? 0.25 : 0.0)
                - student.stress * 0.012
                + randomInt(-2, 2) * 0.4,
            0.0,
            100.0
        );

        student.attendance = clampValue(
            student.attendance
                + (student.satisfaction - 60.0) * 0.015
                - student.fatigue * 0.006
                + randomInt(-2, 2) * 0.3,
            55.0,
            100.0
        );
    }
}

int GameSimulation::processDropouts()
{
    int dropouts = 0;

    for (int i = static_cast<int>(students_.size()) - 1; i >= 0; --i)
    {
        const Student& student = students_.at(i);
        const double feeRatio = tuitionRatioForSubject(student.subject);

        double probability = 0.0;

        if (student.satisfaction < 45.0)
            probability += (45.0 - student.satisfaction) * 0.7;

        if (student.stress > 75.0)
            probability += (student.stress - 75.0) * 0.45;

        if (feeRatio > 1.0)
            probability += (feeRatio - 1.0) * 38.0;

        if (reputation_ < 30)
            probability += (30 - reputation_) * 0.25;

        probability = qBound(0.0, probability, 38.0);

        if (randomInt(1, 100) <= static_cast<int>(std::round(probability)))
        {
            students_.removeAt(i);
            ++dropouts;
        }
    }

    return dropouts;
}

bool GameSimulation::runScheduledExam(WeekSummary& summary)
{
    const int month = currentMonth();
    const int weekOfMonth = currentWeekOfMonth();

    if (weekOfMonth != 4)
        return false;

    if (month != 4 && month != 7 && month != 9 && month != 12)
        return false;

    if (students_.isEmpty())
        return false;

    double sum = 0.0;

    for (Student& student : students_)
    {
        const double nerveAdjustment =
            (student.examNerves - 60) * 0.08;

        const double conditionAdjustment =
            -(student.stress * 0.035)
            -(student.fatigue * 0.025);

        const double randomAdjustment = randomInt(-6, 6);

        student.lastExamScore = clampValue(
            student.score
                + nerveAdjustment
                + conditionAdjustment
                + randomAdjustment,
            0.0,
            100.0
        );

        ++student.examCount;
        sum += student.lastExamScore;
    }

    lastExamAverage_ = sum / static_cast<double>(students_.size());

    summary.examOccurred = true;
    summary.examAverage = lastExamAverage_;
    summary.examName = QString("%1월 내신시험").arg(month);

    return true;
}

void GameSimulation::updateFinance()
{
    constexpr double weeksPerMonth = 4.33;

    qint64 monthlyRevenue = 0;

    for (const Student& student : students_)
        monthlyRevenue += tuitionForSubject(student.subject);

    qint64 monthlyPayroll = 0;

    for (const Teacher& teacher : teachers_)
        monthlyPayroll += teacher.monthlySalary;

    const qint64 monthlyRent =
        4200000
        + facilities_.classrooms * 850000
        + facilities_.floorExpansionLevel * 1700000;

    const qint64 monthlyElectricity =
        580000
        + students_.size() * 8500
        + facilities_.classrooms * 65000
        + facilities_.interiorLevel * 85000
        + facilities_.studyRooms * 120000
        + (facilities_.pantry ? 90000 : 0);

    const qint64 monthlyWater =
        230000
        + students_.size() * 3500
        + (facilities_.pantry ? 65000 : 0);

    const qint64 monthlyMaintenance =
        260000
        + facilities_.usedRoomSlots() * 95000
        + facilities_.interiorLevel * 75000;

    lastFinance_.revenue = static_cast<qint64>(monthlyRevenue / weeksPerMonth);
    lastFinance_.payroll = static_cast<qint64>(monthlyPayroll / weeksPerMonth);
    lastFinance_.rent = static_cast<qint64>(monthlyRent / weeksPerMonth);
    lastFinance_.electricity = static_cast<qint64>(monthlyElectricity / weeksPerMonth);
    lastFinance_.water = static_cast<qint64>(monthlyWater / weeksPerMonth);
    lastFinance_.maintenance = static_cast<qint64>(monthlyMaintenance / weeksPerMonth);

    lastFinance_.net =
        lastFinance_.revenue
        - lastFinance_.payroll
        - lastFinance_.rent
        - lastFinance_.electricity
        - lastFinance_.water
        - lastFinance_.maintenance;

    cash_ += lastFinance_.net;
}

double GameSimulation::averageTuitionRatio() const
{
    double sum = 0.0;
    int count = 0;

    for (const SubjectOffer& offer : subjectOffers_)
    {
        if (!offer.opened)
            continue;

        sum += static_cast<double>(offer.currentFee)
            / static_cast<double>(offer.standardFee);
        ++count;
    }

    return count == 0 ? 1.0 : sum / static_cast<double>(count);
}

void GameSimulation::updateReputation()
{
    const double tuitionRatio = averageTuitionRatio();

    double tuitionEffect = 0.0;

    if (tuitionRatio > 1.0)
        tuitionEffect = -(tuitionRatio - 1.0) * 24.0;
    else
        tuitionEffect = (1.0 - tuitionRatio) * 8.0;

    const double target =
        averageScore() * 0.40
        + averageSatisfaction() * 0.23
        + qMin(15.0, students_.size() * 0.35)
        + facilities_.interiorLevel * 1.8
        + (facilities_.consultationRoom ? 2.0 : 0.0)
        + (facilities_.directorOffice ? 1.0 : 0.0)
        + tuitionEffect;

    const int delta = qBound(
        -3,
        static_cast<int>(std::round((target - reputation_) / 9.0)),
        3
    );

    reputation_ = qBound(0, reputation_ + delta, 100);
}

void GameSimulation::updateRival()
{
    int swing = randomInt(-1, 2);

    if (reputation_ > rival_.reputation + 8)
        swing -= 1;

    rival_.reputation = qBound(20, rival_.reputation + swing, 95);

    if (swing > 0)
        rival_.students += randomInt(0, 2);
    else if (swing < 0)
        rival_.students = qMax(18, rival_.students - randomInt(0, 2));
}

void GameSimulation::resolveWeeklyEvent()
{
    if (students_.size() >= facilities_.capacity() - 2)
    {
        latestEvent_ =
            "상담 문의는 있지만 정원이 거의 찼습니다. 추가 강의실을 검토해야 합니다.";
        return;
    }

    if (!students_.isEmpty() && averageStress() >= 70.0)
    {
        reputation_ = qMax(0, reputation_ - 2);
        latestEvent_ =
            "학생들의 스트레스가 위험 수준입니다. 학부모 사이에서 수업 강도에 대한 이야기가 나오고 있습니다.";
        return;
    }

    if (averageTuitionRatio() > 1.10 && randomInt(1, 100) <= 45)
    {
        reputation_ = qMax(0, reputation_ - 1);
        latestEvent_ =
            "지역 커뮤니티에서 학원비가 비싸다는 반응이 나왔습니다.";
        return;
    }

    if (reputation_ >= 55 && randomInt(1, 100) <= 32)
    {
        latestEvent_ =
            "지역 학부모 커뮤니티에 학원 성과가 소개되었습니다. 다음 주 상담 유입에 도움이 될 수 있습니다.";
        return;
    }

    if (rival_.reputation > reputation_ + 12 && randomInt(1, 100) <= 35)
    {
        latestEvent_ =
            QString("%1이 공격적인 홍보를 시작했습니다. 상담 경쟁이 심해지고 있습니다.")
                .arg(rival_.name);
        return;
    }

    if (randomInt(1, 100) <= 12)
    {
        const qint64 repairCost = 350000 + randomInt(0, 8) * 50000;
        cash_ -= repairCost;

        latestEvent_ =
            QString("시설 소모품과 비품 교체가 발생해 %1원이 추가 지출되었습니다.")
                .arg(repairCost);
        return;
    }

    latestEvent_ = "한 주의 수업이 큰 문제 없이 끝났습니다.";
}

bool GameSimulation::hasFreeRoomSlot() const
{
    return facilities_.usedRoomSlots() < facilities_.roomSlots();
}

bool GameSimulation::upgradeInterior()
{
    const qint64 cost = interiorUpgradeCost();

    if (cash_ < cost)
    {
        latestEvent_ = "인테리어 개선 비용이 부족합니다.";
        return false;
    }

    cash_ -= cost;
    ++facilities_.interiorLevel;
    reputation_ = qMin(100, reputation_ + 1);

    latestEvent_ =
        QString("인테리어를 Lv.%1로 개선했습니다. 학생 만족도와 상담 인상이 좋아집니다.")
            .arg(facilities_.interiorLevel);

    return true;
}

bool GameSimulation::expandFloor()
{
    const qint64 cost = floorExpansionCost();

    if (cash_ < cost)
    {
        latestEvent_ = "학원 면적 확장 비용이 부족합니다.";
        return false;
    }

    cash_ -= cost;
    ++facilities_.floorExpansionLevel;

    latestEvent_ =
        QString("학원 면적을 확장했습니다. 이제 총 %1개의 방을 운영할 수 있습니다.")
            .arg(facilities_.roomSlots());

    return true;
}

bool GameSimulation::addClassroom()
{
    if (!hasFreeRoomSlot())
    {
        latestEvent_ = "빈 공간이 없습니다. 먼저 학원 면적을 확장해야 합니다.";
        return false;
    }

    const qint64 cost = classroomCost();

    if (cash_ < cost)
    {
        latestEvent_ = "강의실 추가 비용이 부족합니다.";
        return false;
    }

    cash_ -= cost;
    ++facilities_.classrooms;

    latestEvent_ =
        QString("강의실을 추가했습니다. 현재 최대 정원은 %1명입니다.")
            .arg(facilities_.capacity());

    return true;
}

bool GameSimulation::buildDirectorOffice()
{
    if (facilities_.directorOffice)
    {
        latestEvent_ = "원장실은 이미 있습니다.";
        return false;
    }

    if (!hasFreeRoomSlot())
    {
        latestEvent_ = "빈 공간이 없습니다. 먼저 학원 면적을 확장해야 합니다.";
        return false;
    }

    if (cash_ < directorOfficeCost())
    {
        latestEvent_ = "원장실 공사 비용이 부족합니다.";
        return false;
    }

    cash_ -= directorOfficeCost();
    facilities_.directorOffice = true;
    latestEvent_ = "원장실을 만들었습니다. 학원 운영 체계와 대외 인상이 조금 좋아집니다.";
    return true;
}

bool GameSimulation::buildConsultationRoom()
{
    if (facilities_.consultationRoom)
    {
        latestEvent_ = "상담실은 이미 있습니다.";
        return false;
    }

    if (!hasFreeRoomSlot())
    {
        latestEvent_ = "빈 공간이 없습니다. 먼저 학원 면적을 확장해야 합니다.";
        return false;
    }

    if (cash_ < consultationRoomCost())
    {
        latestEvent_ = "상담실 공사 비용이 부족합니다.";
        return false;
    }

    cash_ -= consultationRoomCost();
    facilities_.consultationRoom = true;
    latestEvent_ = "상담실을 만들었습니다. 신규 상담 유입과 상담 적합도가 좋아집니다.";
    return true;
}

bool GameSimulation::buildPantry()
{
    if (facilities_.pantry)
    {
        latestEvent_ = "탕비실은 이미 있습니다.";
        return false;
    }

    if (!hasFreeRoomSlot())
    {
        latestEvent_ = "빈 공간이 없습니다. 먼저 학원 면적을 확장해야 합니다.";
        return false;
    }

    if (cash_ < pantryCost())
    {
        latestEvent_ = "탕비실 공사 비용이 부족합니다.";
        return false;
    }

    cash_ -= pantryCost();
    facilities_.pantry = true;
    latestEvent_ = "탕비실을 만들었습니다. 학생과 강사의 피로 관리에 도움이 됩니다.";
    return true;
}

bool GameSimulation::buildStudyRoom()
{
    if (!hasFreeRoomSlot())
    {
        latestEvent_ = "빈 공간이 없습니다. 먼저 학원 면적을 확장해야 합니다.";
        return false;
    }

    if (cash_ < studyRoomCost())
    {
        latestEvent_ = "자습실 공사 비용이 부족합니다.";
        return false;
    }

    cash_ -= studyRoomCost();
    ++facilities_.studyRooms;

    latestEvent_ =
        QString("자습실을 추가했습니다. 현재 자습실은 %1개입니다.")
            .arg(facilities_.studyRooms);

    return true;
}

double GameSimulation::averageScore() const
{
    if (students_.isEmpty())
        return 0.0;

    double sum = 0.0;

    for (const Student& student : students_)
        sum += student.score;

    return sum / static_cast<double>(students_.size());
}

double GameSimulation::averageExamScore() const
{
    int count = 0;
    double sum = 0.0;

    for (const Student& student : students_)
    {
        if (student.examCount <= 0)
            continue;

        sum += student.lastExamScore;
        ++count;
    }

    return count == 0 ? 0.0 : sum / static_cast<double>(count);
}

double GameSimulation::averageSatisfaction() const
{
    if (students_.isEmpty())
        return 0.0;

    double sum = 0.0;

    for (const Student& student : students_)
        sum += student.satisfaction;

    return sum / static_cast<double>(students_.size());
}

double GameSimulation::averageStress() const
{
    if (students_.isEmpty())
        return 0.0;

    double sum = 0.0;

    for (const Student& student : students_)
        sum += student.stress;

    return sum / static_cast<double>(students_.size());
}

int GameSimulation::currentMonth() const
{
    int month = 3 + (week_ - 1) / 4;

    while (month > 12)
        month -= 12;

    return month;
}

int GameSimulation::currentWeekOfMonth() const
{
    return ((week_ - 1) % 4) + 1;
}

QString GameSimulation::dateText() const
{
    const int yearOffset = (3 + (week_ - 1) / 4 - 1) / 12;
    const int year = startYear_ + yearOffset;

    return QString("%1년 %2월 %3주차")
        .arg(year)
        .arg(currentMonth())
        .arg(currentWeekOfMonth());
}

QString GameSimulation::nextAssessmentText() const
{
    static const int examMonths[] = {4, 7, 9, 12};

    const int month = currentMonth();
    const int weekOfMonth = currentWeekOfMonth();

    for (int examMonth : examMonths)
    {
        if (examMonth > month || (examMonth == month && weekOfMonth <= 4))
            return QString("%1월 내신시험").arg(examMonth);
    }

    return "다음 해 4월 내신시험";
}

QString GameSimulation::trainingPlanName(TrainingPlan plan) const
{
    switch (plan)
    {
    case TrainingPlan::ScoreIntensive:
        return "성적 집중";
    case TrainingPlan::Fundamentals:
        return "기초 강화";
    case TrainingPlan::Care:
        return "컨디션 관리";
    case TrainingPlan::Balanced:
    default:
        return "균형 운영";
    }
}

QString GameSimulation::weeklyTip() const
{
    if (students_.isEmpty())
        return "학생 상담을 진행해 첫 수강생을 확보하세요.";

    if (students_.size() >= facilities_.capacity() - 2)
        return "정원이 거의 찼습니다. 빈 공간이 있다면 강의실을 추가하고, 없다면 면적부터 확장하세요.";

    if (averageStress() >= 65.0)
        return "학생 스트레스가 높습니다. 다음 주에는 컨디션 관리 운영을 고려하는 편이 안전합니다.";

    if (averageTuitionRatio() > 1.08)
        return "수강료가 시장 기준보다 높습니다. 수익은 좋지만 상담 감소와 퇴원 위험을 함께 확인하세요.";

    if (applicants_.size() <= 2)
        return "상담 대기 학생이 적습니다. 수강료 인하, 상담실 설치 또는 과목 확장이 유입에 도움이 됩니다.";

    if (subjectOffers_.size() > 1)
    {
        for (int i = 0; i < subjectOffers_.size(); ++i)
        {
            if (canOpenSubject(i))
                return QString("%1 강사를 확보했습니다. 과목을 개설하면 새로운 학생 풀이 열립니다.")
                    .arg(subjectOffers_.at(i).subject);
        }
    }

    return QString("다음 평가는 %1입니다. 학생 상태와 성장 속도를 함께 보며 이번 주 운영 방침을 정하세요.")
        .arg(nextAssessmentText());
}

qint64 GameSimulation::tuitionForSubject(const QString& subject) const
{
    for (const SubjectOffer& offer : subjectOffers_)
    {
        if (offer.subject == subject)
            return offer.currentFee;
    }

    return 0;
}

double GameSimulation::tuitionRatioForSubject(const QString& subject) const
{
    for (const SubjectOffer& offer : subjectOffers_)
    {
        if (offer.subject == subject)
        {
            return static_cast<double>(offer.currentFee)
                / static_cast<double>(offer.standardFee);
        }
    }

    return 1.0;
}

qint64 GameSimulation::interiorUpgradeCost() const
{
    return 2800000 + facilities_.interiorLevel * 1600000;
}

qint64 GameSimulation::floorExpansionCost() const
{
    return 12000000 + facilities_.floorExpansionLevel * 7000000;
}

qint64 GameSimulation::classroomCost() const
{
    return 6500000 + facilities_.classrooms * 1800000;
}

qint64 GameSimulation::directorOfficeCost() const
{
    return 4500000;
}

qint64 GameSimulation::consultationRoomCost() const
{
    return 3800000;
}

qint64 GameSimulation::pantryCost() const
{
    return 2800000;
}

qint64 GameSimulation::studyRoomCost() const
{
    return 5200000 + facilities_.studyRooms * 2200000;
}
