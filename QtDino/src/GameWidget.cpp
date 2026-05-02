#include "GameWidget.h"

#include <QKeyEvent>
#include <QPainter>
#include <QRandomGenerator>

#include <algorithm>

namespace {
constexpr int WindowWidth = 1600;
constexpr int WindowHeight = 800;
constexpr int GroundHeight = 500;
constexpr int JumpVelocity = 1840;
constexpr int FallVelocity = -1840;
constexpr int Gravity = 80;
constexpr QRect HeartRect {20, 700, 200, 60};
constexpr QRect StaminaRect {270, 700, 200, 60};
}

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(WindowWidth, WindowHeight);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);

    resources_.load();
    resetRound(true);

    connect(&timer_, &QTimer::timeout, this, &GameWidget::tick);
    timer_.start(1000 / 60);
}

void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillRect(rect(), QColor(32, 33, 36));

    drawBackgrounds(painter);
    drawPlayer(painter);

    if (!welcome_ && !gameOver_) {
        drawFireballs(painter);
        drawProjectiles(painter);
        drawHud(painter);
    }

    if (welcome_) {
        drawCenterText(painter, "Welcome to Run's Dino!", "Press Space to Start Running!");
    } else if (gameOver_) {
        drawCenterText(painter, "Oh You are Out of HP...", "Press R to Restart!");
    } else if (paused_) {
        drawCenterText(painter, "Oh You Paused the Game...", "Press ESC to Continue!");
    }

    if (!welcome_) {
        painter.setPen(Qt::white);
        painter.setFont(QFont(resources_.fontFamily(), 20));
        painter.drawText(QRect(1260, 20, 100, 32), Qt::AlignRight | Qt::AlignVCenter,
                         QString("%1").arg(score_ / 15, 5, 10, QLatin1Char('0')));
        painter.drawText(QRect(1400, 20, 130, 32), Qt::AlignRight | Qt::AlignVCenter,
                         QString("HI %1").arg(highScore_ / 15, 5, 10, QLatin1Char('0')));
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QWidget::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_Down:
        downPressed_ = true;
        break;
    case Qt::Key_Space:
    case Qt::Key_Up:
        spacePressed_ = true;
        if (welcome_) {
            welcome_ = false;
            speed_ = 2;
        }
        break;
    case Qt::Key_Left:
        player_.setGodMode(resources_, true);
        break;
    case Qt::Key_Right:
        attackPressed_ = true;
        break;
    case Qt::Key_R:
        if (gameOver_) {
            resetRound(false);
        }
        break;
    case Qt::Key_Escape:
        if (!gameOver_ && !welcome_) {
            paused_ = !paused_;
        }
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void GameWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QWidget::keyReleaseEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_Down:
        downPressed_ = false;
        player_.deSprint();
        break;
    case Qt::Key_Space:
    case Qt::Key_Up:
        spacePressed_ = false;
        break;
    case Qt::Key_Left:
        player_.setGodMode(resources_, false);
        break;
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}

void GameWidget::resetRound(bool showWelcome)
{
    speed_ = 2;
    score_ = 0;
    obstacleTimer_ = 0;
    spacePressed_ = false;
    downPressed_ = false;
    attackPressed_ = false;
    gameOver_ = false;
    paused_ = false;
    welcome_ = showWelcome;
    birdTime_ = 0;
    player_.reset();

    backgrounds_ = {{
        {"layerStar", QRect(0, 0, 3200, 800), 2},
        {"layerMoon", QRect(0, 0, 3200, 800), 1},
        {"layerCloud", QRect(0, 0, 3200, 800), 4},
        {"layerGround", QRect(0, 0, 3200, 800), 8},
    }};

    for (auto &projectile : projectiles_) {
        projectile = Projectile {};
    }
    for (auto &fireball : fireballs_) {
        fireball = Fireball {};
    }
}

void GameWidget::tick()
{
    updateBackgrounds();

    if (paused_) {
        update();
        return;
    }

    if (welcome_) {
        speed_ = 0;
        update();
        return;
    }

    if (gameOver_) {
        speed_ = 0;
        update();
        return;
    }

    handleHeldInput();
    updateFireballs();

    if (attackPressed_ && player_.stamina > 0) {
        resources_.play("shoot");
        --player_.stamina;
        for (auto &fireball : fireballs_) {
            if (!fireball.active || fireball.pos.x() > WindowWidth || fireball.pos.y() < 0) {
                fireball.active = true;
                fireball.pos = QRect(220, player_.mouthY() + 20, 50, 50);
                break;
            }
        }
    }
    attackPressed_ = false;

    updateProjectiles();
    maybeSpawnProjectile();

    score_ += speed_ - 1;
    if (player_.speedUp()) {
        score_ += speed_ - 1;
    }
    highScore_ = std::max(highScore_, score_);

    if (score_ > 3000 && score_ < 3010) {
        speed_ = 3;
    }
    if (score_ > 6000 && score_ < 6010) {
        speed_ = 4;
    }

    if (!player_.alive()) {
        speed_ = 0;
        gameOver_ = true;
    }

    update();
}

void GameWidget::updateBackgrounds()
{
    const int backgroundSpeed = paused_ ? 0 : speed_;
    for (auto &background : backgrounds_) {
        background.dest.translate(-backgroundSpeed * background.speedMultiplier, 0);
        if (background.dest.x() <= -WindowWidth) {
            background.dest.moveLeft(0);
        }
    }
}

void GameWidget::updateFireballs()
{
    for (auto &fireball : fireballs_) {
        if (!fireball.active) {
            continue;
        }
        fireball.pos.translate(30, 0);
        if (fireball.pos.x() > 2100 || fireball.pos.y() < 0) {
            fireball.active = false;
        }
    }

    for (auto &fireball : fireballs_) {
        if (!fireball.active) {
            continue;
        }
        for (auto &projectile : projectiles_) {
            if (projectile.active
                && projectile.pos.x() > 0
                && projectile.pos.x() < WindowWidth
                && fireball.pos.intersects(projectile.collisionBox)) {
                fireball.active = false;
                projectile.active = false;
                break;
            }
        }
    }
}

void GameWidget::updateProjectiles()
{
    for (auto &projectile : projectiles_) {
        if (!projectile.active) {
            continue;
        }
        projectile.speedX = speed_ * 8;
        projectile.pos.translate(-projectile.speedX, -projectile.speedY);
        projectile.collisionBox = projectile.pos;
        projectile.angle += projectile.speedRotate;
        if (projectile.angle > 360.0) {
            projectile.angle -= 360.0;
        }
        if (projectile.angle < -360.0) {
            projectile.angle += 360.0;
        }

        if (projectile.animated) {
            ++birdTime_;
            if (birdTime_ > 30) {
                birdTime_ = 0;
            }
            projectile.texture = birdTime_ > 15 ? "bird2" : "bird1";
        }

        if (projectile.collisionBox.intersects(player_.collisionBox)) {
            player_.hit(resources_, projectile.energy);
            if (projectile.energy) {
                projectile.active = false;
            }
        }

        if (projectileAvailable(projectile)) {
            projectile.active = false;
        }
    }
}

void GameWidget::maybeSpawnProjectile()
{
    ++obstacleTimer_;
    if (speed_ <= 0 || obstacleTimer_ <= (180 / speed_)) {
        return;
    }

    const int chance = QRandomGenerator::global()->bounded(1, 11);
    if (chance <= 3) {
        const int index = QRandomGenerator::global()->bounded(1, 10);
        for (auto &projectile : projectiles_) {
            if (!projectile.active || projectileAvailable(projectile)) {
                projectile = summonProjectile(index);
                break;
            }
        }
        obstacleTimer_ = 0;
    }
}

void GameWidget::drawBackgrounds(QPainter &painter)
{
    for (const auto &background : backgrounds_) {
        painter.drawPixmap(background.dest, resources_.texture(background.texture));
    }
}

void GameWidget::drawPlayer(QPainter &painter)
{
    QPixmap pixmap = resources_.texture(player_.currentMotion());
    painter.setOpacity(player_.alpha() / 255.0);
    painter.drawPixmap(player_.pos, pixmap);
    painter.setOpacity(1.0);
}

void GameWidget::drawFireballs(QPainter &painter)
{
    for (const auto &fireball : fireballs_) {
        if (fireball.active) {
            painter.drawPixmap(fireball.pos, resources_.texture("fireball"));
        }
    }
}

void GameWidget::drawProjectiles(QPainter &painter)
{
    for (const auto &projectile : projectiles_) {
        if (!projectile.active) {
            continue;
        }

        painter.save();
        painter.translate(projectile.pos.center());
        painter.rotate(projectile.angle);
        painter.translate(-projectile.pos.center());
        if (projectile.animated) {
            painter.drawPixmap(projectile.pos, resources_.texture(projectile.texture), projectile.frame);
        } else {
            painter.drawPixmap(projectile.pos, resources_.texture(projectile.texture));
        }
        painter.restore();
    }
}

void GameWidget::drawHud(QPainter &painter)
{
    const QString heart = QStringLiteral("heart%1").arg(std::clamp(player_.health, 1, 3));
    const QString stamina = QStringLiteral("stamia%1").arg(std::clamp(player_.stamina, 0, 3));
    painter.drawPixmap(HeartRect, resources_.texture(heart));
    painter.drawPixmap(StaminaRect, resources_.texture(stamina));
}

void GameWidget::drawCenterText(QPainter &painter, const QString &line1, const QString &line2)
{
    painter.setPen(Qt::white);
    painter.setFont(QFont(resources_.fontFamily(), 20));
    painter.drawText(QRect(500, 300, 550, 32), Qt::AlignLeft | Qt::AlignVCenter, line1);
    painter.drawText(QRect(500, 350, 500, 32), Qt::AlignLeft | Qt::AlignVCenter, line2);
}

void GameWidget::handleHeldInput()
{
    if (gameOver_) {
        return;
    }
    if (downPressed_) {
        player_.sprintDown();
    }
    if (spacePressed_) {
        player_.jump(resources_);
    }
}

bool GameWidget::projectileAvailable(const Projectile &projectile) const
{
    return !projectile.active
        || projectile.pos.x() < -300
        || projectile.pos.x() > WindowWidth
        || projectile.pos.y() > WindowHeight
        || projectile.pos.y() < -200;
}

GameWidget::Projectile GameWidget::summonProjectile(int index)
{
    Projectile projectile;
    projectile.active = true;
    projectile.pos = QRect(WindowWidth, WindowHeight, 0, 0);
    projectile.frame = QRect(0, 0, 0, 0);
    projectile.speedX = speed_ * 8;

    if (index == 1 || index == 2) {
        projectile.texture = "bird1";
        projectile.pos = QRect(WindowWidth, QRandomGenerator::global()->bounded(200, 450), 200, 150);
        projectile.frame = QRect(0, 0, 200, 150);
        projectile.collisionBox = QRect(projectile.pos.x() + 40, projectile.pos.y() + 45, 120, 60);
        projectile.animated = true;
        return projectile;
    }
    if (index == 3) {
        projectile.texture = "cactusBig1";
        projectile.pos = QRect(WindowWidth, 500, 100, 200);
    } else if (index == 4) {
        projectile.texture = "cactusBig2";
        projectile.pos = QRect(WindowWidth, 500, 200, 200);
    } else if (index == 5) {
        projectile.texture = "cactusMix";
        projectile.pos = QRect(WindowWidth, 500, 300, 200);
    } else if (index == 6) {
        projectile.texture = "cactusSmall1";
        projectile.pos = QRect(WindowWidth, 580, 60, 120);
    } else if (index == 7) {
        projectile.texture = "cactusSmall2";
        projectile.pos = QRect(WindowWidth, 580, 120, 120);
    } else if (index == 8) {
        projectile.texture = "cactusSmall3";
        projectile.pos = QRect(WindowWidth, 580, 180, 120);
    } else {
        projectile.texture = "energy";
        projectile.pos = QRect(WindowWidth, QRandomGenerator::global()->bounded(150, 450), 50, 50);
        projectile.collisionBox = QRect(projectile.pos.x() + 10, projectile.pos.y() + 10, 30, 30);
        projectile.energy = true;
        return projectile;
    }

    projectile.frame = QRect(0, 0, projectile.pos.width(), projectile.pos.height());
    projectile.collisionBox = projectile.pos;
    return projectile;
}

void GameWidget::PlayerDino::reset()
{
    sprint = false;
    inAir = false;
    failed = false;
    afterHit = false;
    godMode = false;
    velocity = 0;
    health = 3;
    stamina = 3;
    flashTime = 0;
    godModeTime = 0;
    motionRate = 5;
    motionRateCount = 1;
    pos = QRect(20, 500, 200, 200);
    collisionBox = QRect(60, 540, 100, 150);
}

void GameWidget::PlayerDino::jump(ResourceManager &resources)
{
    if (!inAir && !sprint) {
        resources.play("jump");
        velocity = JumpVelocity;
        inAir = true;
    }
}

void GameWidget::PlayerDino::sprintDown()
{
    if (!inAir) {
        sprint = true;
        collisionBox = QRect(20, 600, 200, 100);
        motionRate = 4;
    } else {
        velocity = FallVelocity;
    }
}

void GameWidget::PlayerDino::deSprint()
{
    motionRate = 5;
    sprint = false;
    collisionBox = QRect(60, 540, 100, 150);
}

void GameWidget::PlayerDino::setGodMode(ResourceManager &resources, bool pressed)
{
    if (pressed && stamina > 2 && !godMode) {
        resources.play("god");
        godMode = true;
        stamina = 0;
    }
}

void GameWidget::PlayerDino::hit(ResourceManager &resources, bool energy)
{
    if (energy) {
        if (stamina < 3) {
            ++stamina;
        }
        return;
    }

    if (afterHit || godMode) {
        return;
    }
    flashTime = 0;
    afterHit = true;
    --health;
    resources.play("hurt");
}

bool GameWidget::PlayerDino::alive()
{
    if (health <= 0) {
        failed = true;
        return false;
    }
    return true;
}

bool GameWidget::PlayerDino::speedUp() const
{
    return sprint;
}

int GameWidget::PlayerDino::mouthY() const
{
    return collisionBox.y();
}

int GameWidget::PlayerDino::alpha()
{
    if (godMode) {
        ++godModeTime;
        if (godModeTime > 200) {
            godModeTime = 0;
            afterHit = true;
            godMode = false;
        }
        return 100;
    }
    if (!afterHit) {
        return 255;
    }
    ++flashTime;
    if (flashTime > 80) {
        flashTime = 0;
        afterHit = false;
        return 255;
    }
    return flashTime % 20 < 10 ? 100 : 255;
}

QString GameWidget::PlayerDino::currentMotion()
{
    updatePosition();
    ++motionRateCount;
    if (motionRateCount >= 2 * motionRate) {
        motionRateCount = 1;
    }

    const int rate = motionRateCount / motionRate;
    if (failed) {
        return "fail";
    }
    if (inAir) {
        return "idle";
    }
    if (sprint) {
        return rate == 0 ? "sprint1" : "sprint2";
    }
    return rate == 0 ? "run1" : "run2";
}

void GameWidget::PlayerDino::updatePosition()
{
    if (!velocity && !inAir) {
        return;
    }

    const int delta = velocity / 60;
    if (pos.y() - delta > GroundHeight) {
        collisionBox.moveTop(GroundHeight + 40);
        pos.moveTop(GroundHeight);
        inAir = false;
        velocity = 0;
        return;
    }

    pos.translate(0, -delta);
    collisionBox.translate(0, -delta);
    velocity -= Gravity;
}
