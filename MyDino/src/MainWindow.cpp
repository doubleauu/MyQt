#include "MainWindow.h"

// 重载构造函数，固定长和宽
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(900, 500);
    setWindowTitle("MyDino");
}
