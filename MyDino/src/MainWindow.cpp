#include "MainWindow.h"
#include "GameWidget.h"

// 构造函数实现，固定长和宽
MainWindow::MainWindow(QWidget *parent) // 默认没有父类窗口
    : QMainWindow(parent)  // 初始化列表写法，先初始化父类 QMainWindow
{
    resize(1600, 800);  // 等价于 this->resize()
    setWindowTitle("MyDino");

    // 主窗口一般只作为一个载体，需要更改就加新组件
    setCentralWidget(new GameWidget(this)); // 将游戏窗口放到主窗口中心，并以主窗口为父类
}
