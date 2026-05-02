#include "MainWindow.h"

#include "GameWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Runz's Dino");
    setFixedSize(1600, 800);
    setCentralWidget(new GameWidget(this));
}
