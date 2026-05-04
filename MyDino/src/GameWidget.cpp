#include "GameWidget.h"

#include <QPainter>

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
}

// 界面刷新函数实现
void GameWidget::tick() {  // 作用域限定符写在返回类型后面
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

    // 简易小恐龙：
    painter.setBrush(QColor(80,200,120));  // 设置填充颜色
    painter.setPen(Qt::NoPen);  // 设置无边框
    painter.drawRect(100,600,80,100);  // 绘制矩形
}
