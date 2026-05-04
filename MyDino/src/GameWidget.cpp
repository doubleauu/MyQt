#include "GameWidget.h"

#include <QPainter>
#include <QKeyEvent>
#include <QCoreApplication>  // 应用程序本体的基础功能类
#include <algorithm>
// 字体相关类库：
#include <QFontDatabase>
#include <QStringList>

// 匿名命名空间，这些变量只在内部使用，不属于成员变量，不需要暴露，表示规则常量
// constexpr 表示翻译期常量，强于 const
namespace {
constexpr int GroundY = 500;  // 恐龙在地面时左上角 y 坐标
constexpr int JumpVelocity = 1840;  // 起跳初始速度
constexpr int Gravity = 80;  // 重力加速度
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

    // 仙人掌照片：
    cactusPixmap_.load(imagePath + "Cactus_SMALL1.png");

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

// 界面刷新函数实现
void GameWidget::tick() {  // 作用域限定符写在返回类型后面

    // 失败判断：
    if (gameOver_) {
        update();
        return;
    }

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

    // 背景移动：
    groundOffset_ -= scrollSpeed_;
    if(groundOffset_ <= -40) {
        groundOffset_ = 0;
    }

    // 仙人掌移动
    obstacleRect_.translate(-obstacleSpeed_,0);
    if(obstacleRect_.right() < 0) {  // 重复利用
        obstacleRect_.moveLeft(width());
    }

    // 添加小恐龙碰撞框，创建新的矩形，直接复用原版参数
    QRect dinoCollisionBox(
        dinoRect_.x() + 40,
        dinoRect_.y() + 40,
        100,150
    );

    if(dinoCollisionBox.intersects(obstacleRect_)) {
        gameOver_ = true;
    }


    // 更新分数，每一帧加一分，之后显示的时候再缩小，不然不好实现
    if(!gameOver_) {  // 这里进行二次判断，是防止这一帧刚碰撞还继续加分
        score_ += 1;
        highScore_ = std::max(highScore_, score_);
    }

    // 更新小恐龙跑步动画：
    if(!inAir_) {
        ++motionRateCount_;
        if(motionRateCount_ >= 5) {  // 每五帧更新一次图片
            motionRateCount_ = 0;
            currentRunFrame_ = 1 - currentRunFrame_;
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

    // 地面：
    painter.setPen(QPen(Qt::white,3));  // 设置画笔颜色和宽度
    painter.drawLine(0,700,width(),700);  // 设置直线两端坐标

    // 地面小线段
    painter.setPen(QPen(QColor(180,180,180),2));
    for(int x = groundOffset_; x < width(); x+=40) {  //每隔 40 像素重复生成，与背景最大偏移量周期一致
        painter.drawLine(x,700,x+20,720);  // 画一条斜线
    }

    // 仙人掌障碍物：
    if(!cactusPixmap_.isNull()) {
        painter.drawPixmap(obstacleRect_,cactusPixmap_);  // 第一个参数是目标矩形（位置），第二个参数是填充图片
    }else {  // 未加载图片就画简易矩形替代
        painter.setBrush(QColor(200, 80, 80));
        painter.setPen(Qt::NoPen);
        painter.drawRect(obstacleRect_);
    }

    // 小恐龙
    const QPixmap *currentPixmap = nullptr;  // 指针指向的对象内容不能修改，但是可以更改指向内容
    if(inAir_) {
        currentPixmap = &idlePixmap_;
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

    //重置障碍物和背景位置：
    obstacleRect_.moveLeft(width());
    groundOffset_ = 0;

    // 重置小恐龙动画：
    motionRateCount_ = 0;
    currentRunFrame_ = 0;

    // 重置游戏状态：
    gameOver_ = false;
    score_ = 0;
}

// 键盘接受函数：
void GameWidget::keyPressEvent(QKeyEvent *event) {

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

    // 跳跃按键
    if(event->key()==Qt::Key_Space || event->key()==Qt::Key_Up || event->key()==Qt::Key_W) {
        if(!inAir_) {
            velocityY_ = JumpVelocity;  // 初始速度，后续随重力加速度变化
            inAir_ = true;
        }else {
            QWidget::keyPressEvent(event);
        }
    }else {
        QWidget::keyPressEvent(event);  // 其他按键不处理；
    }



}
