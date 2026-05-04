#pragma once

#include <QWidget>
#include <QTimer>

class GameWidget : public QWidget {  // 继承自 QWitget 类
public:
    explicit GameWidget(QWidget *parent = nullptr);  // 构造函数声明
private:
    QTimer timer_; // 计时器，负责每隔一段时间提醒程序刷新
    void tick();  // 游戏每一帧要执行的函数，即每一帧的更新入口
};
