#pragma once

#include "GameSimulation.h"

#include <QPointF>
#include <QTimer>
#include <QVector>
#include <QWidget>

class AcademySceneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AcademySceneWidget(QWidget* parent = nullptr);

    void setSimulation(const GameSimulation* simulation);
    void refreshScene();
    void startClassAnimation();
    bool isAnimating() const { return classAnimating_; }

signals:
    void classAnimationFinished();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    struct Sprite
    {
        QString name;
        QPointF position;
        QPointF target;
        int deskIndex = 0;
        int step = 0;
    };

    void syncSprites();
    void chooseIdleTarget(Sprite& sprite);
    void tick();

    void drawScene(QPainter& painter);
    void drawRoom(
        QPainter& painter,
        const QRectF& room,
        const QString& label,
        const QString& type,
        int roomIndex
    );
    void drawClassroom(QPainter& painter, const QRectF& room, int classroomIndex);
    void drawStudent(QPainter& painter, const Sprite& sprite, int index);
    void drawOwner(QPainter& painter);
    QRectF classroomRectForIndex(int index) const;
    QPointF deskPositionForStudent(int studentIndex) const;

    const GameSimulation* simulation_ = nullptr;

    QVector<Sprite> sprites_;
    QTimer timer_;

    bool classAnimating_ = false;
    int animationTick_ = 0;
    int animationPhase_ = 0;
};
