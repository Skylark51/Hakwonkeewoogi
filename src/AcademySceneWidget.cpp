#include "AcademySceneWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <QResizeEvent>

#include <cmath>

namespace
{
constexpr double SceneWidth = 960.0;
constexpr double SceneHeight = 500.0;

QColor floorColor(int level)
{
    if (level >= 4)
        return QColor(224, 205, 170);
    if (level >= 2)
        return QColor(215, 195, 164);
    return QColor(202, 185, 158);
}
}

AcademySceneWidget::AcademySceneWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(420);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    timer_.setInterval(90);

    connect(&timer_, &QTimer::timeout, this, [this] {
        tick();
    });

    timer_.start();
}

void AcademySceneWidget::setSimulation(const GameSimulation* simulation)
{
    simulation_ = simulation;
    syncSprites();
    update();
}

void AcademySceneWidget::refreshScene()
{
    syncSprites();
    update();
}

void AcademySceneWidget::startClassAnimation()
{
    if (!simulation_ || classAnimating_)
        return;

    syncSprites();

    classAnimating_ = true;
    animationTick_ = 0;
    animationPhase_ = 0;

    for (int i = 0; i < sprites_.size(); ++i)
        sprites_[i].target = deskPositionForStudent(i);

    update();
}

void AcademySceneWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}

void AcademySceneWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    painter.fillRect(rect(), QColor(31, 41, 48));

    const double scale =
        qMin(width() / SceneWidth, height() / SceneHeight);

    const double offsetX = (width() - SceneWidth * scale) / 2.0;
    const double offsetY = (height() - SceneHeight * scale) / 2.0;

    painter.translate(offsetX, offsetY);
    painter.scale(scale, scale);

    drawScene(painter);
}

void AcademySceneWidget::syncSprites()
{
    if (!simulation_)
    {
        sprites_.clear();
        return;
    }

    const auto& students = simulation_->students();

    while (sprites_.size() > students.size())
        sprites_.removeLast();

    while (sprites_.size() < students.size())
    {
        const int index = static_cast<int>(sprites_.size());

        Sprite sprite;
        sprite.name = students.at(index).name;
        sprite.position = QPointF(
            430.0 + QRandomGenerator::global()->bounded(180),
            400.0 + QRandomGenerator::global()->bounded(45)
        );
        sprite.deskIndex = index;
        chooseIdleTarget(sprite);

        sprites_.push_back(sprite);
    }

    for (int i = 0; i < sprites_.size(); ++i)
    {
        sprites_[i].name = students.at(i).name;
        sprites_[i].deskIndex = i;
    }
}

void AcademySceneWidget::chooseIdleTarget(Sprite& sprite)
{
    const int classroomCount =
        simulation_ ? qMax(1, simulation_->facilities().classrooms) : 1;

    const int classroomIndex =
        sprite.deskIndex % classroomCount;

    const QRectF room = classroomRectForIndex(classroomIndex);

    const double x =
        room.left() + 35.0
        + QRandomGenerator::global()->bounded(
            qMax(1, static_cast<int>(room.width() - 70.0))
        );

    const double y =
        room.top() + 65.0
        + QRandomGenerator::global()->bounded(
            qMax(1, static_cast<int>(room.height() - 90.0))
        );

    sprite.target = QPointF(x, y);
}

void AcademySceneWidget::tick()
{
    if (!simulation_)
        return;

    syncSprites();

    if (classAnimating_)
    {
        ++animationTick_;

        if (animationTick_ < 22)
        {
            animationPhase_ = 0;

            for (int i = 0; i < sprites_.size(); ++i)
                sprites_[i].target = deskPositionForStudent(i);
        }
        else if (animationTick_ < 48)
        {
            animationPhase_ = 1;
        }
        else if (animationTick_ < 62)
        {
            animationPhase_ = 2;

            for (Sprite& sprite : sprites_)
                chooseIdleTarget(sprite);
        }
        else
        {
            classAnimating_ = false;
            animationPhase_ = 0;
            animationTick_ = 0;

            for (Sprite& sprite : sprites_)
                chooseIdleTarget(sprite);

            update();
            emit classAnimationFinished();
            return;
        }
    }

    for (Sprite& sprite : sprites_)
    {
        const QPointF delta = sprite.target - sprite.position;
        const double distance =
            std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (distance < 3.0)
        {
            if (!classAnimating_)
                chooseIdleTarget(sprite);

            continue;
        }

        double speed = classAnimating_ ? 7.0 : 2.1;

        if (animationPhase_ == 1)
            speed = 0.0;

        if (speed > 0.0)
        {
            sprite.position += QPointF(
                delta.x() / distance * speed,
                delta.y() / distance * speed
            );
        }

        ++sprite.step;
    }

    update();
}

QRectF AcademySceneWidget::classroomRectForIndex(int index) const
{
    const double roomWidth = 270.0;
    const double roomHeight = 185.0;

    const int col = index % 3;
    const int row = (index / 3) % 2;

    return QRectF(
        35.0 + col * 300.0,
        35.0 + row * 215.0,
        roomWidth,
        roomHeight
    );
}

QPointF AcademySceneWidget::deskPositionForStudent(int studentIndex) const
{
    if (!simulation_)
        return QPointF(120.0, 120.0);

    const int classroomCount =
        qMax(1, simulation_->facilities().classrooms);

    const int classroomIndex =
        studentIndex % classroomCount;

    const int localIndex =
        studentIndex / classroomCount;

    const QRectF room =
        classroomRectForIndex(classroomIndex);

    const int col = localIndex % 4;
    const int row = (localIndex / 4) % 3;

    return QPointF(
        room.left() + 55.0 + col * 52.0,
        room.top() + 85.0 + row * 34.0
    );
}

void AcademySceneWidget::drawScene(QPainter& painter)
{
    if (!simulation_)
        return;

    const FacilityState& facilities = simulation_->facilities();

    painter.fillRect(
        QRectF(10, 10, 940, 480),
        QColor(64, 78, 84)
    );

    painter.fillRect(
        QRectF(20, 20, 920, 455),
        floorColor(facilities.interiorLevel)
    );

    painter.setPen(QPen(QColor(166, 148, 124), 1));

    for (int x = 20; x < 940; x += 24)
        painter.drawLine(x, 20, x, 475);

    for (int y = 20; y < 475; y += 24)
        painter.drawLine(20, y, 940, y);

    int roomIndex = 0;

    for (int i = 0; i < facilities.classrooms; ++i)
    {
        drawRoom(
            painter,
            classroomRectForIndex(roomIndex),
            QString("강의실 %1").arg(i + 1),
            "classroom",
            i
        );
        ++roomIndex;
    }

    auto specialRect = [roomIndex](int specialIndex) {
        const int absoluteIndex = roomIndex + specialIndex;
        const int col = absoluteIndex % 3;
        const int row = (absoluteIndex / 3) % 2;

        return QRectF(
            35.0 + col * 300.0,
            35.0 + row * 215.0,
            270.0,
            185.0
        );
    };

    int specialIndex = 0;

    if (facilities.directorOffice)
    {
        drawRoom(
            painter,
            specialRect(specialIndex++),
            "원장실",
            "director",
            0
        );
    }

    if (facilities.consultationRoom)
    {
        drawRoom(
            painter,
            specialRect(specialIndex++),
            "상담실",
            "consultation",
            0
        );
    }

    if (facilities.pantry)
    {
        drawRoom(
            painter,
            specialRect(specialIndex++),
            "탕비실",
            "pantry",
            0
        );
    }

    for (int i = 0; i < facilities.studyRooms; ++i)
    {
        drawRoom(
            painter,
            specialRect(specialIndex++),
            QString("자습실 %1").arg(i + 1),
            "study",
            i
        );
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(85, 105, 112));
    painter.drawRect(QRectF(20, 440, 920, 35));

    painter.setPen(QColor(235, 240, 242));
    QFont hallFont = painter.font();
    hallFont.setBold(true);
    hallFont.setPointSize(9);
    painter.setFont(hallFont);
    painter.drawText(QRectF(35, 446, 890, 20), Qt::AlignCenter, "학원 복도");

    drawOwner(painter);

    for (int i = 0; i < sprites_.size(); ++i)
        drawStudent(painter, sprites_.at(i), i);

    if (classAnimating_)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(25, 36, 43, 205));
        painter.drawRoundedRect(QRectF(350, 445, 260, 32), 5, 5);

        painter.setPen(QColor(255, 255, 255));

        QString phaseText = "학생들이 강의실로 이동 중...";

        if (animationPhase_ == 1)
            phaseText = "수업 진행 중 · 필기 · 문제풀이";
        else if (animationPhase_ == 2)
            phaseText = "수업 종료 · 쉬는 시간";

        painter.drawText(
            QRectF(360, 450, 240, 20),
            Qt::AlignCenter,
            phaseText
        );
    }
}

void AcademySceneWidget::drawRoom(
    QPainter& painter,
    const QRectF& room,
    const QString& label,
    const QString& type,
    int roomIndex
)
{
    Q_UNUSED(roomIndex);

    painter.setPen(QPen(QColor(86, 69, 57), 5));
    painter.setBrush(QColor(238, 225, 201));
    painter.drawRect(room);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(113, 82, 58));
    painter.drawRect(QRectF(room.left() + 6, room.bottom() - 13, 40, 13));

    painter.setPen(QColor(73, 57, 47));
    QFont labelFont = painter.font();
    labelFont.setBold(true);
    labelFont.setPointSize(9);
    painter.setFont(labelFont);

    painter.drawText(
        QRectF(room.left() + 8, room.top() + 7, room.width() - 16, 24),
        Qt::AlignLeft | Qt::AlignVCenter,
        label
    );

    if (type == "classroom")
    {
        drawClassroom(painter, room, roomIndex);
        return;
    }

    if (type == "director")
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(112, 77, 50));
        painter.drawRect(QRectF(room.left() + 75, room.top() + 82, 120, 45));

        painter.setBrush(QColor(69, 102, 117));
        painter.drawRect(QRectF(room.left() + 112, room.top() + 59, 45, 25));

        painter.setBrush(QColor(89, 124, 76));
        painter.drawRect(QRectF(room.right() - 42, room.top() + 54, 22, 45));
    }
    else if (type == "consultation")
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(127, 92, 60));
        painter.drawEllipse(QRectF(room.left() + 78, room.top() + 75, 110, 65));

        painter.setBrush(QColor(89, 112, 126));
        painter.drawRect(QRectF(room.left() + 48, room.top() + 90, 22, 36));
        painter.drawRect(QRectF(room.right() - 70, room.top() + 90, 22, 36));
    }
    else if (type == "pantry")
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(194, 198, 190));
        painter.drawRect(QRectF(room.left() + 35, room.top() + 62, 190, 38));

        painter.setBrush(QColor(114, 153, 171));
        painter.drawRect(QRectF(room.right() - 70, room.top() + 112, 38, 55));

        painter.setBrush(QColor(210, 88, 69));
        painter.drawRect(QRectF(room.left() + 70, room.top() + 112, 25, 25));
    }
    else if (type == "study")
    {
        painter.setPen(Qt::NoPen);

        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                const double x = room.left() + 38 + col * 52;
                const double y = room.top() + 62 + row * 35;

                painter.setBrush(QColor(137, 93, 57));
                painter.drawRect(QRectF(x, y, 36, 18));
            }
        }
    }
}

void AcademySceneWidget::drawClassroom(
    QPainter& painter,
    const QRectF& room,
    int classroomIndex
)
{
    Q_UNUSED(classroomIndex);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(49, 84, 70));
    painter.drawRect(QRectF(room.left() + 55, room.top() + 37, 160, 23));

    painter.setBrush(QColor(119, 79, 47));

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            const double x = room.left() + 38 + col * 52;
            const double y = room.top() + 78 + row * 34;

            painter.drawRect(QRectF(x, y, 38, 18));
        }
    }

    painter.setBrush(QColor(89, 103, 112));
    painter.drawRect(QRectF(room.left() + 18, room.top() + 50, 28, 34));
}

void AcademySceneWidget::drawStudent(
    QPainter& painter,
    const Sprite& sprite,
    int index
)
{
    const QPointF pos = sprite.position;

    const int palette = index % 5;

    const QColor bodyColors[] = {
        QColor(78, 121, 167),
        QColor(232, 126, 75),
        QColor(89, 161, 79),
        QColor(176, 122, 161),
        QColor(237, 201, 72)
    };

    const double bob =
        (classAnimating_ && animationPhase_ == 1)
        ? ((sprite.step / 3) % 2 == 0 ? 1.5 : 0.0)
        : 0.0;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(238, 198, 160));
    painter.drawRect(QRectF(pos.x() - 5, pos.y() - 14 - bob, 10, 9));

    painter.setBrush(bodyColors[palette]);
    painter.drawRect(QRectF(pos.x() - 7, pos.y() - 5 - bob, 14, 13));

    painter.setBrush(QColor(55, 59, 66));
    painter.drawRect(QRectF(pos.x() - 6, pos.y() + 8 - bob, 5, 7));
    painter.drawRect(QRectF(pos.x() + 1, pos.y() + 8 - bob, 5, 7));

    painter.setPen(QColor(38, 45, 49));
    QFont nameFont = painter.font();
    nameFont.setPointSize(7);
    painter.setFont(nameFont);

    painter.drawText(
        QRectF(pos.x() - 30, pos.y() + 17, 60, 14),
        Qt::AlignCenter,
        sprite.name
    );
}

void AcademySceneWidget::drawOwner(QPainter& painter)
{
    if (!simulation_)
        return;

    const QRectF firstRoom = classroomRectForIndex(0);
    const QPointF pos(
        firstRoom.left() + 28,
        firstRoom.top() + 102
    );

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(226, 190, 150));
    painter.drawRect(QRectF(pos.x() - 6, pos.y() - 17, 12, 10));

    painter.setBrush(QColor(58, 70, 91));
    painter.drawRect(QRectF(pos.x() - 8, pos.y() - 7, 16, 17));

    painter.setPen(QColor(35, 42, 48));
    QFont font = painter.font();
    font.setPointSize(7);
    font.setBold(true);
    painter.setFont(font);

    painter.drawText(
        QRectF(pos.x() - 30, pos.y() + 12, 60, 15),
        Qt::AlignCenter,
        "원장"
    );
}
