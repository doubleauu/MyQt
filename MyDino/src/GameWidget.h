#pragma once

#include <QWidget>
#include <QTimer>
#include <QPixmap>  // Qt 中用来保存图片的类

class GameWidget : public QWidget {  // 继承自 QWitget 类
public:
    explicit GameWidget(QWidget *parent = nullptr);  // 构造函数声明

protected:
    void paintEvent(QPaintEvent *event) override;  // 界面绘制函数
    void keyPressEvent(QKeyEvent *event) override; // 键盘输入函数
private:
    QTimer timer_; // 计时器，负责每隔一段时间提醒程序刷新
    void tick();  // 游戏每一帧要执行的函数，即每一帧的更新入口

    QRect dinoRect_ {20,500,200,200};  // 恐龙的位置和大小
    int velocityY_ = 0;  // 竖直方向速度
    bool inAir_ = false; // 是否在空中，防止无限连跳

    int groundOffset_ = 0;  // 背景偏移量
    int scrollSpeed_ = 6;  // 背景偏移速度

    // 存放小恐龙图片
    QPixmap run1Pixmap_;
    QPixmap run2Pixmap_;
    QPixmap idlePixmap_;  // 跳跃状态

    int motionRateCount_ = 0;  // 计数器，控制切图时机
    int currentRunFrame_ = 0;  // 显示当前为第几张图片

    // 添加仙人掌障碍物：
    QPixmap cactusPixmap_;
    QRect obstacleRect_ {1600,580,60,120};
    int obstacleSpeed_ = 16;

    // 添加结束状态：
    bool gameOver_ = false;

};
