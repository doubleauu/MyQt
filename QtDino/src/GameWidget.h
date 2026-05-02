#pragma once

#include "ResourceManager.h"

#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>

#include <array>

class GameWidget : public QWidget
{
public:
    explicit GameWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    struct BackgroundLayer {
        QString texture;
        QRect dest;
        int speedMultiplier = 1;
    };

    struct PlayerDino {
        bool sprint = false;
        bool inAir = false;
        bool failed = false;
        bool afterHit = false;
        bool godMode = false;
        int velocity = 0;
        int health = 3;
        int stamina = 3;
        int flashTime = 0;
        int godModeTime = 0;
        int motionRate = 5;
        int motionRateCount = 1;
        QRect pos {20, 500, 200, 200};
        QRect collisionBox {60, 540, 100, 150};

        void reset();
        void jump(ResourceManager &resources);
        void sprintDown();
        void deSprint();
        void setGodMode(ResourceManager &resources, bool pressed);
        void hit(ResourceManager &resources, bool energy);
        bool alive();
        bool speedUp() const;
        int mouthY() const;
        int alpha();
        QString currentMotion();
        void updatePosition();
    };

    struct Projectile {
        bool active = false;
        bool animated = false;
        bool energy = false;
        QString texture;
        QRect pos;
        QRect frame;
        QRect collisionBox;
        int speedX = 0;
        int speedY = 0;
        int speedRotate = 0;
        double angle = 0.0;
    };

    struct Fireball {
        bool active = false;
        QRect pos;
    };

    void resetRound(bool showWelcome);
    void tick();
    void updateBackgrounds();
    void updateFireballs();
    void updateProjectiles();
    void maybeSpawnProjectile();
    void drawBackgrounds(QPainter &painter);
    void drawPlayer(QPainter &painter);
    void drawFireballs(QPainter &painter);
    void drawProjectiles(QPainter &painter);
    void drawHud(QPainter &painter);
    void drawCenterText(QPainter &painter, const QString &line1, const QString &line2);
    void handleHeldInput();
    bool projectileAvailable(const Projectile &projectile) const;
    Projectile summonProjectile(int index);

    ResourceManager resources_;
    QTimer timer_;
    std::array<BackgroundLayer, 4> backgrounds_;
    PlayerDino player_;
    std::array<Projectile, 5> projectiles_;
    std::array<Fireball, 3> fireballs_;
    bool spacePressed_ = false;
    bool downPressed_ = false;
    bool attackPressed_ = false;
    bool gameOver_ = false;
    bool welcome_ = true;
    bool paused_ = false;
    int speed_ = 2;
    int score_ = 0;
    int highScore_ = 0;
    int obstacleTimer_ = 0;
    int birdTime_ = 0;
};
