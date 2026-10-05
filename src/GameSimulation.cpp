#include "GameSimulation.h"

#include <QRandomGenerator>
#include <QtGlobal>

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
    rival_.specialty = "영어";
}

void GameSimulation::setupAcademy(const QString& academyName, const QString& specialty)
{
    academyName_ = academyName.trimmed().isEmpty() ? "새싹 학원" : academyName.trimmed();
    specialty_ = specialty;

    students_.clear();
    applicants_.clear();
    teachers_.clear();
    teacherCandidates_.clear();

    facilities_ = FacilityState{};
    lastFinance_ = FinanceSnapshot{};

    week_ = 1;
    reputation_ = 35;
    cash_ = 35000000;

    rival_.name = "탑클래스 학원";
    rival_.specialty = specialty_ == "수학" ? "영어" : "수학";
    rival_.reputation = 48;
    rival_.students = 42;
    rival_.monthlyTuition = 450000;

    generateApplicants(6);
    generateTeacherCandidates(5);

    latestEvent_ =
        QString("%1이(가) 개원했습니다. 아직 강사는 없으며 원장이 %2 수업을 직접 맡습니다.")
            .arg(academyName_, specialty_);
}

int GameSimulation::randomInt(int lowInclusive, int highInclusive) const
{
    return lowInclusive
        + QRandomGenerator::global()->bounded(highInclusive - lowInclusive + 1);
}

void GameSimulation::generateApplicants(int targetCount)
{
    static const QStringList surnames = {
        "김", "이", "박", "최", "정", "강", "조", "윤", "장", "임", "한"
    };
    static const QStringList givenNames = {
        "민준", "서준", "도윤", "지호", "예준", "서연", "지우", "하윤",
        "민서", "지민", "수빈", "현우", "유진", "채원", "시우", "예은",
        "준영", "서현", "은우", "나연", "태윤", "다은"
    };
    static const QStringList subjects = {"수학", "영어", "국어", "과학"};

    while (applicants_.size() < targetCount)
    {
        Applicant applicant;
        Student& student = applicant.student;

        student.name =
            surnames.at(randomInt(0, surnames.size() - 1))
            + givenNames.at(randomInt(0, givenNames.size() - 1));

        student.schoolYear = randomInt(1, 3);

        const bool wantsSpecialty = randomInt(1, 100) <= 58;
        student.subject = wantsSpecialty
            ? specialty_
            : subjects.at(randomInt(0, subjects.size() - 1));

        student.score = randomInt(42, 88);
        student.potential = randomInt(50, 96);
        student.diligence = randomInt(35, 95);
        student.comprehension = randomInt(40, 96);
        student.memorization = randomInt(40, 96);
        student.concentration = randomInt(35, 95);
        student.examNerves = randomInt(30, 95);

        student.stress = randomInt(10, 62);
        student.fatigue = randomInt(8, 50);
        student.satisfaction = randomInt(55, 85);
        student.attendance = randomInt(80, 100);
        student.monthlyTuition = 390000 + randomInt(0, 6) * 10000;

        applicant.consultationFit = randomInt(40, 96);
        applicant.parentDemand = randomInt(20, 95);

        if (student.potential >= 88)
            applicant.note = "성장 잠재력이 매우 높음";
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

void GameSimulation::generateTeacherCandidates(int targetCount)
{
    static const QStringList surnames = {
        "김", "이", "박", "최", "정", "오", "송", "백", "문", "신"
    };
    static const QStringList givenNames = {
        "도현", "서윤", "준호", "민재", "유나", "현석", "지혜", "성민",
        "가영", "승현", "태훈", "지은", "예린", "동현", "수진"
    };
    static const QStringList subjects = {"수학", "영어", "국어", "과학"};
    static const QStringList traits = {
        "스타강사형", "엄격한 관리형", "친근한 상담형",
        "연구·교재형", "체력 좋은 열정형", "무난한 균형형"
    };

    while (teacherCandidates_.size() < targetCount)
    {
        Teacher teacher;

        teacher.name =
            surnames.at(randomInt(0, surnames.size() - 1))
            + givenNames.at(randomInt(0, givenNames.size() - 1));

        teacher.subject = randomInt(1, 100) <= 45
            ? specialty_
            : subjects.at(randomInt(0, subjects.size() - 1));

        teacher.teaching = randomInt(52, 92);
        teacher.counseling = randomInt(40, 92);
        teacher.discipline = randomInt(40, 92);
        teacher.charisma = randomInt(35, 95);
        teacher.stamina = randomInt(45, 95);

        teacher.trait = traits.at(randomInt(0, traits.size() - 1));

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
        latestEvent_ = "정원이 가득 찼습니다. 강의실을 확장해야 신규 학생을 받을 수 있습니다.";
        return false;
    }

    const Applicant applicant = applicants_.at(index);
    students_.push_back(applicant.student);
    applicants_.removeAt(index);

    latestEvent_ =
        QString("상담 결과 %1 학생을 등록시켰습니다. %2 수강을 시작합니다.")
            .arg(applicant.student.name, applicant.student.subject);

    generateApplicants(6);
    return true;
}

bool GameSimulation::rejectApplicant(int index)
{
    if (index < 0 || index >= applicants_.size())
        return false;

    const QString name = applicants_.at(index).student.name;
    applicants_.removeAt(index);

    latestEvent_ = QString("%1 학생의 등록을 받지 않기로 결정했습니다.").arg(name);

    generateApplicants(6);
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
        QString("%1 %2 강사를 채용했습니다. 특징: %3")
            .arg(candidate.name, candidate.subject, candidate.trait);

    generateTeacherCandidates(5);
    return true;
}

double GameSimulation::teacherEffectFor(const QString& subject) const
{
    double best = subject == specialty_ ? 62.0 : 46.0; // 원장 직강 기본치

    for (const Teacher& teacher : teachers_)
    {
        double value =
            teacher.teaching * 0.68
            + teacher.discipline * 0.14
            + teacher.counseling * 0.10
            + teacher.stamina * 0.08;

        if (teacher.subject != subject)
            value *= 0.72;

        best = qMax(best, value);
    }

    return best;
}

void GameSimulation::advanceWeek(TrainingPlan plan)
{
    updateStudents(plan);
    updateFinance();
    updateReputation();
    updateRival();
    resolveWeeklyEvent();

    ++week_;

    if (applicants_.size() < 6)
        generateApplicants(6);

    if (teacherCandidates_.size() < 5)
        generateTeacherCandidates(5);
}

void GameSimulation::updateStudents(TrainingPlan plan)
{
    for (Student& student : students_)
    {
        const double teacherEffect = teacherEffectFor(student.subject);
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

        const double conditionPenalty =
            student.stress * 0.0035 + student.fatigue * 0.003;

        const double randomFactor = randomInt(-12, 12) / 100.0;

        double growth =
            (
                0.12
                + teacherEffect * 0.010
                + talentFactor
                + disciplineFactor
                - conditionPenalty
                + randomFactor
            ) * growthMultiplier;

        if (student.score > student.potential)
            growth *= 0.55;

        if (student.score >= 95.0)
            growth *= 0.30;

        student.score = clampValue(student.score + growth, 0.0, 100.0);

        const double counselingRelief =
            qMax(0.0, teacherEffectFor(student.subject) - 60.0) * 0.015;

        student.stress = clampValue(
            student.stress + stressDelta - counselingRelief + randomInt(-2, 2),
            0.0,
            100.0
        );

        student.fatigue = clampValue(
            student.fatigue + fatigueDelta + randomInt(-2, 2),
            0.0,
            100.0
        );

        student.satisfaction = clampValue(
            student.satisfaction
                + satisfactionDelta
                + growth * 0.35
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

void GameSimulation::updateFinance()
{
    constexpr double weeksPerMonth = 4.33;

    qint64 monthlyRevenue = 0;
    for (const Student& student : students_)
        monthlyRevenue += student.monthlyTuition;

    qint64 monthlyPayroll = 0;
    for (const Teacher& teacher : teachers_)
        monthlyPayroll += teacher.monthlySalary;

    const qint64 monthlyRent =
        4200000 + facilities_.classrooms * 1250000;

    const qint64 monthlyElectricity =
        650000
        + students_.size() * 8500
        + facilities_.interiorLevel * 90000
        + facilities_.studyRoomLevel * 140000;

    const qint64 monthlyWater =
        260000 + students_.size() * 3500;

    const qint64 monthlyMaintenance =
        280000
        + facilities_.classrooms * 120000
        + facilities_.interiorLevel * 80000;

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

void GameSimulation::updateReputation()
{
    const double target =
        averageScore() * 0.46
        + averageSatisfaction() * 0.25
        + qMin(15.0, students_.size() * 0.35)
        + facilities_.interiorLevel * 2.0
        + facilities_.studyRoomLevel * 1.5;

    const int delta = qBound(
        -2,
        static_cast<int>((target - reputation_) / 10.0),
        2
    );

    reputation_ = qBound(0, reputation_ + delta, 100);
}

void GameSimulation::updateRival()
{
    const int swing = randomInt(-1, 2);

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
            "상담 문의는 늘고 있지만 정원이 거의 찼습니다. 강의실 확장을 검토해야 합니다.";
        return;
    }

    if (!students_.isEmpty() && averageStress() >= 70.0)
    {
        reputation_ = qMax(0, reputation_ - 2);
        latestEvent_ =
            "학생들의 스트레스가 위험 수준입니다. 일부 학부모가 수업 강도를 낮춰 달라고 요청했습니다.";
        return;
    }

    if (reputation_ >= 55 && randomInt(1, 100) <= 32)
    {
        generateApplicants(8);
        latestEvent_ =
            "지역 학부모 커뮤니티에 학원 성과가 소개되었습니다. 상담 문의가 증가했습니다.";
        return;
    }

    if (rival_.reputation > reputation_ + 12 && randomInt(1, 100) <= 35)
    {
        latestEvent_ =
            QString("%1이 공격적인 홍보를 시작했습니다. 다음 상담 시즌에 경쟁이 심해질 수 있습니다.")
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

    latestEvent_ =
        QString("%1 방식으로 한 주를 운영했습니다. 큰 문제 없이 일정이 마무리되었습니다.")
            .arg("선택한 트레이닝");
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
        QString("인테리어를 Lv.%1로 개선했습니다. 상담 인상과 학원 평판에 도움이 됩니다.")
            .arg(facilities_.interiorLevel);

    return true;
}

bool GameSimulation::expandClassroom()
{
    const qint64 cost = classroomExpansionCost();

    if (cash_ < cost)
    {
        latestEvent_ = "강의실 확장 비용이 부족합니다.";
        return false;
    }

    cash_ -= cost;
    ++facilities_.classrooms;

    latestEvent_ =
        QString("강의실을 확장했습니다. 현재 최대 정원은 %1명입니다.")
            .arg(facilities_.capacity());

    return true;
}

bool GameSimulation::upgradeStudyRoom()
{
    const qint64 cost = studyRoomUpgradeCost();

    if (cash_ < cost)
    {
        latestEvent_ = "자습실 투자 비용이 부족합니다.";
        return false;
    }

    cash_ -= cost;
    ++facilities_.studyRoomLevel;

    for (Student& student : students_)
        student.satisfaction = clampValue(student.satisfaction + 1.5, 0.0, 100.0);

    latestEvent_ =
        QString("자습실을 Lv.%1로 개선했습니다. 학생 만족도가 소폭 상승했습니다.")
            .arg(facilities_.studyRoomLevel);

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

qint64 GameSimulation::interiorUpgradeCost() const
{
    return 3500000 + facilities_.interiorLevel * 1750000;
}

qint64 GameSimulation::classroomExpansionCost() const
{
    return 9000000 + facilities_.classrooms * 4500000;
}

qint64 GameSimulation::studyRoomUpgradeCost() const
{
    return 4500000 + facilities_.studyRoomLevel * 3000000;
}
