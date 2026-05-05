#pragma once

#include <QWidget>
#include <QTimer>
#include <QPixmap>  // Qt 中用来保存图片的类
#include <QString>
#include <QSoundEffect>

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

    // 添加分层背景图片资源
    QPixmap starPixmap_;
    QPixmap moonPixmap_;
    QPixmap cloudPixmap_;
    QPixmap groundPixmap_;

    // 每一层背景的绘制区域：
    QRect starRect_ {0,0,3200,800};
    QRect moonRect_ {0,0,3200,800};
    QRect cloudRect_ {0,0,3200,800};
    QRect groundRect_ {0,0,3200,800};

    // 存放小恐龙图片
    QPixmap run1Pixmap_;
    QPixmap run2Pixmap_;
    QPixmap idlePixmap_;  // 跳跃状态
    // 下蹲状态图片：
    QPixmap sprint1Pixmap_;
    QPixmap sprint2Pixmap_;

    int motionRateCount_ = 0;  // 计数器，控制切图时机
    int currentRunFrame_ = 0;  // 显示当前为第几张图片

    void spawnNextObstacle();  // 随机生成下一个障碍物

    // 添加多种类型仙人掌障碍物：
    QPixmap cactusSmall1Pixmap_;
    QPixmap cactusSmall2Pixmap_;
    QPixmap cactusSmall3Pixmap_;
    QPixmap cactusBig1Pixmap_;
    QPixmap cactusBig2Pixmap_;
    QPixmap cactusMixPixmap_;
    QRect obstacleRect_ {1600,580,60,120};
    int obstacleSpeed_ = 16;

    // 添加飞鸟资源和障碍物类型，还有能量球
    enum class ObstacleType {
        CactusSmall1,
        CactusSmall2,
        CactusSmall3,
        CactusBig1,
        CactusBig2,
        CactusMix,
        Bird,
        Energy
    };
    ObstacleType obstacleType_ = ObstacleType::CactusSmall1;  // 当前障碍物类型，默认小仙人掌

    //能量球图片：
    QPixmap energyballPixmap_;

    // 飞鸟照片和计数器：
    QPixmap bird1Pixmap_;
    QPixmap bird2Pixmap_;
    int birdFrameCount_ = 0;  // 小鸟动画计数器，判断应该画哪一帧动画
    int currentBirdFrame_ = 0;  // 当前显示图片

    // 游戏基础状态：
    bool welcome_ = true;
    bool paused_ = false;
    bool gameOver_ = false;

    // 无敌状态：
    bool godMode_ = false;
    int godModeFrames_ = 0;  // 无敌帧数

    // 生命值和受伤保护：
    int health_ = 3;  // 初始值为 3
    int hurtProtectFrames_ = 0;
    // 生命值图片：
    QPixmap heart1Pixmap_;
    QPixmap heart2Pixmap_;
    QPixmap heart3Pixmap_;

    // 体力值和体力图片：
    int stamina_ = 3;
    QPixmap stamina0Pixmap_;
    QPixmap stamina1Pixmap_;
    QPixmap stamina2Pixmap_;
    QPixmap stamina3Pixmap_;

    // 火球:
    QPixmap fireballPixmap_;
    bool fireballActive_ = false;
    QRect fireballRect_ {0, 0, 50, 50};  // 初始位置，没什么用，反正后面会更新

    // 添加当前分数和最高分显示
    int score_ = 0;
    int highScore_ = 0;

    // 保存字体名称，使用默认字体兜底
    QString fontFamily_ = QStringLiteral("Consolas");

    // 音效：
    QSoundEffect jumpSound_;
    QSoundEffect hurtSound_;
    QSoundEffect shootSound_;
    QSoundEffect godSound_;
    QSoundEffect scoreSound_;
    // 防止分数音效每一帧重复播放
    bool score3000Played_ = false;
    bool score6000Played_ = false;

};
