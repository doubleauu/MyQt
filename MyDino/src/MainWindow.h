#pragma once   // 保证这个头文件在一次编译过程中只被包含一次，防止引入多次导致重定义

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    // explicit 关键字，禁止构造函数发生隐式类型转换
    // 构造函数前面常常加 explicit，是更安全、更规范的写法。
    explicit MainWindow(QWidget *parent = nullptr);  // 构造函数声明
};
