cmake_minimum_required(VERSION 3.24) 	# cmake最低支持版本

project(MyDino VERSION 1.0 LANGUAGES CXX)   # 定义项目名称

set(CMAKE_CXX_STANDARD 17)   #  指定C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)  #  standard标识强制使用
set(CMAKE_AUTOMOC ON)  #  自动处理qt特性

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)  # 寻找系统中安装的库


#  添加源文件，将这些文件一起编写成一个程序
qt_add_executable(MyDino
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/GameWidget.h
    src/GameWidget.cpp
)

#  把 Qt6 的 Widgets 模块库 链接到可执行目标 MyDino 上（窗口功能）
#  加上 Multimedia 库，支持音频
#  private 表示权限，private只对当前目标生效
target_link_libraries(MyDino PRIVATE Qt6::Widgets Qt6::Multimedia)

# 链接图片，音频资源文件：每次构建后，把 MyDino/Resources 复制到 exe 旁边。
add_custom_command(TARGET MyDino POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:MyDino>/Resources
)
