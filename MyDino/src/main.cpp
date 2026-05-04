#include "MainWindow.h"

#include <QApplication>  // 引入 Qt 的应用程序类。

// argc, argv 是命令行参数，前者表示数量
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // 创建 Qt 应用对象，负责所有qt事件的管理

    MainWindow window;
    window.show();

    return app.exec();  //进入 Qt 的事件循环，等待用户操作
}
