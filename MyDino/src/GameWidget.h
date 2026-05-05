#pragma once

#include <QWidget>
#include <QTimer>
#include <QPixmap>  // Qt 中用来保存图片的类
#include <QString>

class GameWidget : public QWidget {  // 继承自 QWitget 类
public:
    explicit GameWidget(QWidget *parent = nullptr);  // 构造函数声明

protected:
    void paintEvent(QPaintEvent *event) override;  // 界面绘制函数
    void keyPressEvent(QKeyEvent *event) override; // 键盘输入函数
    void keyReleaseEvent(QKeyEvent *event) override;  // 下蹲按键触发
private:
    QTimer timer_; // 计时器，负责每隔一段时间提醒程序刷新
    void tick();  // 游戏每一帧要执行的函数，即每一帧的更新入口
    void resetGame();  // 重新开始游戏

    QRect dinoRect_ {20,500,200,200};  // 恐龙的位置和大小
    int velocityY_ = 0;  // 竖直方向速度
    bool inAir_ = false; // 是否在空中，防止无限连跳
    bool sprint_ = false;  // 是否下蹲冲刺
    bool downPressed_ = false;  // 是否持续冲刺
    bool spacePressed_ = false;  // 是否持续尝试跳跃

    int groundOffset_ = 0;  // 背景偏移量
    int speed_ = 2;  // 当前游戏速度档位，分数越高，速度越快
    int scrollSpeed_ = 6;  // 背景偏移速度

    // 存放小恐龙图片
    QPixmap run1Pixmap_;
    QPixmap run2Pixmap_;
    QPixmap idlePixmap_;  // 跳跃状态
    // 下蹲状态图片：
    QPixmap sprint1Pixmap_;
    QPixmap sprint2Pixmap_;

    int motionRateCount_ = 0;  // 计数器，控制切图时机
    int currentRunFrame_ = 0;  // 显示当前为第几张图片

    // 添加仙人掌障碍物：
    QPixmap cactusPixmap_;
    QRect obstacleRect_ {1600,580,60,120};
    int obstacleSpeed_ = 16;

    // 添加飞鸟资源和障碍物类型
    enum class ObstacleType {
        Cactus,
        Bird
    };

    QPixmap bird1Pixmap_;
    QPixmap bird2Pixmap_;
    ObstacleType obstacleType_ = ObstacleType::Cactus;  // 当前障碍物类型
    int birdFrameCount_ = 0;  // 小鸟动画计数器，判断应该画哪一帧动画
    int currentBirdFrame_ = 0;  // 当前显示图片

    // 添加结束状态：
    bool gameOver_ = false;

    // 添加当前分数和最高分显示
    int score_ = 0;
    int highScore_ = 0;

    // 保存字体名称，使用默认字体兜底
    QString fontFamily_ = QStringLiteral("Consolas");
};
