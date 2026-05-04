#include "GameWidget.h"

#include <QPainter>
#include <QKeyEvent>
#include <QCoreApplication>  // 应用程序本体的基础功能类

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
    const QString imagePath = QCoreApplication::applicationDirPath() + "/Resources/Textures/Run1.png";
    dinoPixmap_.load(imagePath);
}

// 界面刷新函数实现
void GameWidget::tick() {  // 作用域限定符写在返回类型后面

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

    // 小恐龙
    if(!dinoPixmap_.isNull()) {  // 如果图片加载成功了就绘制图片
        painter.drawPixmap(dinoRect_, dinoPixmap_);
    }else {  // 未加载成功还绘制简易版矩形代替小恐龙
        painter.setBrush(QColor(80,200,120));  // 设置填充颜色
        painter.setPen(Qt::NoPen);  // 设置无边框
        painter.drawRect(dinoRect_);  // 绘制矩形
    }
}

// 键盘接受函数：
void GameWidget::keyPressEvent(QKeyEvent *event) {
    if(event->key()==Qt::Key_Space || event->key()==Qt::Key_Up || event->key()==Qt::Key_W) {
        if(!inAir_) {
            velocityY_ = JumpVelocity;  // 初始速度，后续随重力加速度变化
            inAir_ = true;
        }else {
            QWidget::keyPressEvent(event);
        }
    }
}
