#include "GameWidget.h"

#include <QPainter>
#include <QKeyEvent>
#include <QCoreApplication>  // 应用程序本体的基础功能类
#include <algorithm>
// 字体相关类库：
#include <QFontDatabase>
#include <QStringList>

#include <QRandomGenerator>  // 用于生成随机数

// 匿名命名空间，这些变量只在内部使用，不属于成员变量，不需要暴露，表示规则常量
// constexpr 表示翻译期常量，强于 const
namespace {
constexpr int GroundY = 500;  // 恐龙在地面时左上角 y 坐标
constexpr int JumpVelocity = 1840;  // 起跳初始速度
constexpr int Gravity = 80;  // 重力加速度
constexpr int FallVelocity = -1840;  // 空中下蹲快速下落
}

// 构造函数实现：
GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)  // 依旧先初始化父类对象
{
    setFixedSize(1600,800);  // 固定游戏界面大小
    setFocusPolicy(Qt::StrongFocus);  // 设置焦点策略，使其可以接受键盘输入（焦点：谁在接收键盘输入）

    // 建立信号与槽的连接，配合定时器，周期性调用 tick(),效果相当于一个循环
    // 当 timer_ 发出 timeout 信号，就调用当前对象的 tick（）
    // timer 是一个对象，此处要传地址，因为 connect 需要知道哪个对象发出信号
    connect(&timer_, &QTimer::timeout, this, &GameWidget::tick);  // 连接定时器 timer_ 与刷新函数 tick()
    timer_.start(1000/60);  //启动定时器，每隔约 16 毫秒触发一次 timeout 信号，约等于 60 帧率

    // 加载小恐龙图片
    // Path 函数返回 .exe 文件所在路径，cmake构建会自动将资源文件复制到 exe 旁边
    const QString imagePath = QCoreApplication::applicationDirPath() + "/Resources/Textures/";
    run1Pixmap_.load(imagePath + "Run1.png");
    run2Pixmap_.load(imagePath + "Run2.png");
    idlePixmap_.load(imagePath + "Idle.png");
    sprint1Pixmap_.load(imagePath + "Sprint1.png");
    sprint2Pixmap_.load(imagePath + "Sprint2.png");

    // 仙人掌照片：
    cactusSmall1Pixmap_.load(imagePath + "Cactus_SMALL1.png");
    cactusSmall2Pixmap_.load(imagePath + "Cactus_SMALL2.png");
    cactusSmall3Pixmap_.load(imagePath + "Cactus_SMALL3.png");
    cactusBig1Pixmap_.load(imagePath + "Cactus_BIG1.png");
    cactusBig2Pixmap_.load(imagePath + "Cactus_BIG2.png");
    cactusMixPixmap_.load(imagePath + "Cactus_MIX.png");

    // 小鸟照片：
    bird1Pixmap_.load(imagePath + "Bird1.png");
    bird2Pixmap_.load(imagePath + "Bird2.png");

    // 背景照片：
    starPixmap_.load(imagePath + "LAYER_STAR.png");
    moonPixmap_.load(imagePath + "LAYER_MOON.png");
    cloudPixmap_.load(imagePath + "LAYER_CLOUD.png");
    groundPixmap_.load(imagePath + "LAYER_GROUND.png");

    // 加载字体：
    const QString resourcePath = QCoreApplication::applicationDirPath() + "/Resources/";
    const int fontId  = QFontDatabase::addApplicationFont(resourcePath + "TEXTS.ttf");  // 把字体文件注册到当前 Qt 程序中
    if(fontId >= 0) {  // 如果加载成功（返回一个编号）：失败是返回-1；
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId); // 根据编号获取字体名称列表
        if(!families.isEmpty()) {   // 如果列表不为空，就取列表中第一个字体名称存入 fontFamily_
            fontFamily_ = families.first();
        }
    }
}

// 界面刷新函数实现，每一帧都会执行一次
void GameWidget::tick() {  // 作用域限定符写在返回类型后面

    // 失败判断：
    if (gameOver_) {
        update();
        return;
    }

    // 根据分数更新速度档位
    if(score_ >= 6000) {
        speed_ = 4;
    }else if (score_ >= 3000) {
        speed_ = 3;
    }else {
        speed_ = 2;
    }

    scrollSpeed_ = speed_ * 3;  // 背景滚动速度
    obstacleSpeed_ = speed_ * 8;   // 障碍物移动速度

    // 小恐龙跳跃：
    if(inAir_) {  // 如果在空中，就更新跳跃运动
        const int delta = velocityY_ / 60;  // 这一帧恐龙在 Y 轴上要移动多少像素。= 每秒速度/每秒帧数

        if(dinoRect_.y() - delta > GroundY) {  // 当落到地面上时，更新状态，可以进行下一次跳跃；
            dinoRect_.moveTop(GroundY);  // 将矩形顶部强制放回 groundY;
            velocityY_ = 0;
            inAir_ = false;
        }else {
            dinoRect_.translate(0,-delta); // 在原位置基础上移动恐龙矩形，x 方向不动，y 方向移动 velocityY_（速度）
            velocityY_ -= Gravity;  // 速度随重力加速度变化，向上时逐渐变小，向下时逐步变大
        }
    }

    // 每一帧持续判断小恐龙是否冲刺状态，避免落地后卡顿
    if(downPressed_) {
        if(inAir_) {
            velocityY_ = FallVelocity;
            sprint_ = false;
        }else {
            sprint_ = true;
        }
    }else {
        sprint_ = false;
    }

    // 每一帧持续判断小恐龙是否尝试跳跃，避免下蹲期间的跳跃按键无响应
    if(spacePressed_ && !inAir_ && !sprint_) {
        velocityY_ = JumpVelocity;  // 初始速度，后续随重力加速度变化
        inAir_ =  true;
    }

    // 分层背景移动：不同层背景移动速率不同
    starRect_.translate(-speed_ * 2,0);
    moonRect_.translate(-speed_ * 1,0);
    cloudRect_.translate(-speed_ * 4,0);
    groundRect_.translate(-speed_ * 8,0);
    // 移动出窗口时回正
    if(starRect_.x() <= -1600) {
        starRect_.moveLeft(0);
    }
    if(moonRect_.x() <= -1600) {
        moonRect_.moveLeft(0);
    }
    if(cloudRect_.x() <= -1600) {
        cloudRect_.moveLeft(0);
    }
    if(groundRect_.x() <= -1600) {
        groundRect_.moveLeft(0);
    }

    // 障碍物移动
    obstacleRect_.translate(-obstacleSpeed_,0);
    // 更换随机障碍物：
    if(obstacleRect_.right() < 0) {
        spawnNextObstacle();
    }

    // 可变碰撞框：创建新的矩形，直接复用原版参数
    QRect dinoCollisionBox;
    if(sprint_ && !inAir_) {
        dinoCollisionBox = QRect(
            dinoRect_.x(),   // 小恐龙下蹲后变宽的部分恰好等于 40 像素，所以此处不用加
            dinoRect_.y()+100,  // 变低了 60 像素
            200,100
        );
    }else {
        dinoCollisionBox = QRect(
            dinoRect_.x()+40,  // 图片本身有透明区域，所以碰撞框范围要减小一些
            dinoRect_.y()+40,
            100,150
        );
    }

    // 小鸟的碰撞检测区域较小，创建新的矩形
    QRect obstacleCollisionBox = obstacleRect_;  // 默认使用仙人掌碰撞矩形
    if(obstacleType_ == ObstacleType::Bird) {
        obstacleCollisionBox = QRect (
            obstacleRect_.x()+40,
            obstacleRect_.y()+45,
            120,60
        );
    }

    if(dinoCollisionBox.intersects(obstacleCollisionBox)) {
        gameOver_ = true;
    }


    // 更新分数，每一帧加一分，之后显示的时候再缩小，不然不好实现
    if(!gameOver_) {  // 这里进行二次判断，是防止这一帧刚碰撞还继续加分
        score_ += speed_ - 1;
        if(sprint_) score_ += speed_ - 1;  // 冲刺状态加分
        highScore_ = std::max(highScore_, score_);
    }

    // 更新小恐龙跑步动画：
    if(!inAir_) {
        ++motionRateCount_;
        const int frameInterval = sprint_ ? 4 : 5;  // 下蹲状态就每 4 帧更新一次照片
        if(motionRateCount_ >= frameInterval) {  // 每五帧更新一次图片
            motionRateCount_ = 0;
            currentRunFrame_ = 1 - currentRunFrame_;
        }
    }

    // 更新飞鸟动画：
    if(obstacleType_ == ObstacleType::Bird) {
        ++birdFrameCount_;

        if(birdFrameCount_ >= 15) {  // 每 15 帧更新一次图片
            birdFrameCount_ = 0;
            currentBirdFrame_ = 1 - currentBirdFrame_;
        }
    }

    update(); // qt的函数，用来 “请求” 每一帧界面的刷新，之后会触发 paintEvent()
}

// 实现界面绘制函数
// 当需要重画界面是自动调用，变量名可以省略，因为此处不会使用 event
void GameWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);  // 创建画笔，属于当前窗口

    // 背景：
    painter.fillRect(rect(),QColor(32,33,36));  // 填充整个矩形界面
    // 分层画背景：
    painter.drawPixmap(starRect_, starPixmap_);
    painter.drawPixmap(moonRect_, moonPixmap_);
    painter.drawPixmap(cloudRect_, cloudPixmap_);
    painter.drawPixmap(groundRect_, groundPixmap_);

    // 绘制障碍物，代码写法同小恐龙，先写一个空指针，然后判断指向哪一个图片，最后进行绘制
    const QPixmap *obstaclePixmap = nullptr;

    switch(obstacleType_) {
    case ObstacleType::CactusSmall1:
        obstaclePixmap = &cactusSmall1Pixmap_;
        break;
    case ObstacleType::CactusSmall2:
        obstaclePixmap = &cactusSmall2Pixmap_;
        break;
    case ObstacleType::CactusSmall3:
        obstaclePixmap = &cactusSmall3Pixmap_;
        break;
    case ObstacleType::CactusBig1:
        obstaclePixmap = &cactusBig1Pixmap_;
        break;
    case ObstacleType::CactusBig2:
        obstaclePixmap = &cactusBig2Pixmap_;
        break;
    case ObstacleType::CactusMix:
        obstaclePixmap = &cactusMixPixmap_;
        break;
    case ObstacleType::Bird:  // 飞鸟障碍物吗，需要动画判断
        obstaclePixmap = currentBirdFrame_ == 0 ? &bird1Pixmap_ : &bird2Pixmap_;
        break;
    }

    if(obstaclePixmap && !obstaclePixmap->isNull()) {  // 前者判断是否空指针，后者判断图片是否加载成功
        painter.drawPixmap(obstacleRect_, *obstaclePixmap);  // 注意指针需要解引用，第一个参数是目标矩形（位置），第二个参数是填充图片
    }

    // 小恐龙
    const QPixmap *currentPixmap = nullptr;  // 指针指向的对象内容不能修改，但是可以更改指向内容
    if(inAir_) {
        currentPixmap = &idlePixmap_;
    }else if(sprint_) {  // 处理小恐龙下蹲时的动画帧数直接复用之前的判断变量：currentRunFrame_，保持一致
        if(currentRunFrame_ == 0) currentPixmap = &sprint1Pixmap_;
        else currentPixmap = &sprint2Pixmap_;
    }else if(currentRunFrame_ == 0) {
        currentPixmap = &run1Pixmap_;
    }else {
        currentPixmap = &run2Pixmap_;
    }

    if(currentPixmap && !currentPixmap->isNull()) {  // 如果图片加载成功了就绘制图片
        painter.drawPixmap(dinoRect_, *currentPixmap);
    }else {  // 未加载成功还绘制简易版矩形代替小恐龙
        painter.setBrush(QColor(80,200,120));  // 设置填充颜色
        painter.setPen(Qt::NoPen);  // 设置无边框
        painter.drawRect(dinoRect_);  // 绘制矩形
    }

    // 分数绘制：
    painter.setPen(Qt::white);
    painter.setFont(QFont(fontFamily_,20));
    painter.drawText(
        QRect(1260,20,100,32),
        Qt::AlignRight | Qt::AlignVCenter,  // 靠右，且上下居中
        // 构造字符串对象：生成分数字符串 %1 是占位符，等待 arg 中的结果， 5 表示长度，10 表示进制，0 表示不足前面补 0
        QString("%1").arg(score_ / 15, 5, 10, QLatin1Char('0'))  //减小显示的分数，防止增长过快
    );
    painter.drawText(
        QRect(1400, 20, 130, 32),
        Qt::AlignRight | Qt::AlignVCenter,
        QString("HI %1").arg(highScore_ / 15, 5, 10, QLatin1Char('0'))
    );

    // 失败提示：
    if(gameOver_) {
        painter.setPen(Qt::white);
        painter.setFont(QFont(fontFamily_,32));
        painter.drawText(rect(),Qt::AlignCenter,"Game Over\nPress R to Restart");
    }

}

// 重新开始函数：
void GameWidget::resetGame() {
    // 重置小恐龙状态：
    dinoRect_.moveTop(GroundY);
    velocityY_ = 0;
    inAir_ = false;
    sprint_ = false;
    downPressed_ = false;
    spacePressed_ = false;

    //重置障碍物和背景位置：
    obstacleRect_.moveLeft(width());
    groundOffset_ = 0;
    starRect_.moveLeft(0);
    moonRect_.moveLeft(0);
    cloudRect_.moveLeft(0);
    groundRect_.moveLeft(0);

    // 重置小恐龙动画：
    motionRateCount_ = 0;
    currentRunFrame_ = 0;

    // 重置飞鸟动画：
    birdFrameCount_ = 0;
    currentBirdFrame_ = 0;

    // 重置游戏状态：
    gameOver_ = false;
    score_ = 0;

    // 重置速度：
    speed_ = 2;
    scrollSpeed_ = speed_ * 3;
    obstacleSpeed_ = speed_ * 8;
}

// 键盘接受函数：
void GameWidget::keyPressEvent(QKeyEvent *event) {

    // 删除系统产生的自动重复按键事件，防止界面闪烁
    if(event->isAutoRepeat()) {
        QWidget::keyPressEvent(event);
        return;
    }

    // 重开按键：
    if(event->key()==Qt::Key_R && gameOver_) {
        resetGame();
        return;
    }

    // 游戏结束后不再处理按键
    if(gameOver_) {
        QWidget::keyPressEvent(event);
        return;
    }

    // 只记录是否按下按键，具体情况交给 tick() 刷新函数判断
    // 下蹲冲刺按键：支持 down 和 s 键
    if(event->key()==Qt::Key_Down || event->key()==Qt::Key_S) {
        downPressed_ = true;
        return;
    }

    // 跳跃按键
    if(event->key()==Qt::Key_Space || event->key()==Qt::Key_Up || event->key()==Qt::Key_W) {
        spacePressed_ = true;
        return;  // 其他按键不处理
    }

}

// 持续下蹲冲刺释放按键，松开按键时触发
void GameWidget::keyReleaseEvent(QKeyEvent *event) {

    // 删除系统产生的自动重复按键事件，防止界面闪烁
    if(event->isAutoRepeat()) {
        QWidget::keyReleaseEvent(event);
        return;
    }

    if(event->key()==Qt::Key_Down || event->key()==Qt::Key_S) {
        downPressed_ = false;
        sprint_ = false;
        return;
    }
    if(event->key()==Qt::Key_Space || event->key()==Qt::Key_Up || event->key()==Qt::Key_W) {
        spacePressed_ = false;
        return;
    }
    QWidget::keyReleaseEvent(event);  // 其他按键丢该父类，也就是不处理；
}

// 随机障碍物
void GameWidget::spawnNextObstacle() {
    const int typeIndex = QRandomGenerator::global()->bounded(0,7);
    obstacleType_ = static_cast<ObstacleType>(typeIndex);

    birdFrameCount_ = 0;
    currentBirdFrame_ = 0;

    // 对于不同类型障碍物，更改他们的 图片所在矩形
    switch(obstacleType_) {
    case ObstacleType::CactusSmall1:
        obstacleRect_ = QRect(width(), 580, 60, 120);
        break;
    case ObstacleType::CactusSmall2:
        obstacleRect_ = QRect(width(), 580, 120, 120);
        break;
    case ObstacleType::CactusSmall3:
        obstacleRect_ = QRect(width(), 580, 180, 120);
        break;
    case ObstacleType::CactusBig1:
        obstacleRect_ = QRect(width(), 500, 100, 200);
        break;
    case ObstacleType::CactusBig2:
        obstacleRect_ = QRect(width(), 500, 200, 200);
        break;
    case ObstacleType::CactusMix:
        obstacleRect_ = QRect(width(), 500, 300, 200);
        break;
    case ObstacleType::Bird: {  // 飞鸟随机生成高度
        const int birdY = QRandomGenerator::global()->bounded(200, 450);
        obstacleRect_ = QRect(width(), birdY, 200, 150);
        break;
    }
    }
}
