# QtDino 第一版迁移说明

## 项目简介

`QtDino` 是从仓库中 `Dino/` 目录下的 C/SDL2 版恐龙跑酷小游戏迁移而来的 C++/Qt Widgets 版本。  
当前版本的目标不是重新设计游戏，而是先完成一版可构建、可启动、可初步游玩的 Qt 复现版本，为后续课程复现、代码讲解和进一步优化打基础。

原版项目主要使用：

- C11
- SDL2
- SDL2_image
- SDL2_ttf
- Windows `winmm` 音效播放

迁移后的 `QtDino` 使用：

- C++17
- Qt 6 Widgets
- Qt Multimedia
- CMake
- Ninja

## 当前迁移效果

第一版已经基本复刻原 SDL 版的主要玩法和视觉效果：

- 固定窗口大小：`1600 x 800`
- 多层背景滚动
- 恐龙跑步、跳跃、冲刺、受击闪烁、失败状态
- 障碍物生成与移动，包括仙人掌、飞鸟和能量球
- 火球发射与命中障碍物
- 生命值 UI
- 体力值 UI
- 分数与最高分显示
- 欢迎界面
- 暂停界面
- 游戏失败界面
- 游戏结束后重新开始
- 跳跃、受击、无敌、射击等音效

当前版本可以认为是“第一版初步迁移”：玩法已经基本与原版一致，后续的美化、结构重构、手感微调可以暂时搁置，继续围绕最重要的复现内容推进。

## 环境要求

本项目已经在以下环境中配置并构建通过：

- Qt：`C:\Qt\6.5.3\mingw_64`
- Qt Creator Kit：`Desktop Qt 6.5.3 MinGW 64-bit`
- CMake：`C:\Qt\Tools\CMake_64\bin\cmake.exe`
- Ninja：`C:\Qt\Tools\Ninja\ninja.exe`
- MinGW：`C:\Qt\Tools\mingw1120_64`
- 编译器：`g++ 11.2.0`

如果本机 Qt 安装路径不同，需要在 Qt Creator 或 CMake 参数中调整对应路径。

## 在 Qt Creator 中运行

推荐使用 Qt Creator 打开和运行项目：

1. 打开 Qt Creator。
2. 选择 `文件` -> `打开文件或项目`。
3. 打开：

   ```text
   C:\workspace\Qt\QtDino\CMakeLists.txt
   ```

4. 选择 Kit：

   ```text
   Desktop Qt 6.5.3 MinGW 64-bit
   ```

5. 点击 `Configure Project` / `配置项目`。
6. 左下角确认运行目标为 `QtDino`。
7. 点击绿色运行按钮，或按 `Ctrl + R`。

如果 Qt Creator 没有识别到 Kit，可以在：

```text
工具 -> 选项 -> Kits
```

中检查这些路径：

- Qt version：`C:\Qt\6.5.3\mingw_64\bin\qmake.exe`
- Compiler：`C:\Qt\Tools\mingw1120_64\bin\g++.exe`
- CMake：`C:\Qt\Tools\CMake_64\bin\cmake.exe`
- Ninja：`C:\Qt\Tools\Ninja\ninja.exe`

## 命令行构建

也可以在仓库根目录 `C:\workspace\Qt` 下使用命令行构建：

```powershell
C:\Qt\Tools\CMake_64\bin\cmake.exe `
  -S QtDino `
  -B QtDino/build `
  -G Ninja `
  -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/Ninja/ninja.exe `
  -DCMAKE_PREFIX_PATH=C:/Qt/6.5.3/mingw_64 `
  -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1120_64/bin/g++.exe `
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
```

然后执行：

```powershell
C:\Qt\Tools\CMake_64\bin\cmake.exe --build QtDino/build
```

构建完成后可以运行：

```text
C:\workspace\Qt\QtDino\build\QtDino.exe
```

`CMakeLists.txt` 中已经配置了构建后步骤，会把 `Resources/` 复制到可执行文件目录，并在 Windows 下调用 `windeployqt` 部署 Qt 运行所需的 DLL 和插件。

## 目录结构

```text
QtDino/
├── CMakeLists.txt          # Qt6/CMake 构建配置
├── README.md               # 当前说明文档
├── Resources/              # 游戏资源，来自原 Dino/bin/Resources
│   ├── TEXTS.ttf           # 字体
│   ├── Audios/             # WAV 音效
│   └── Textures/           # 背景、恐龙、障碍物、UI、火球等图片
├── src/
│   ├── main.cpp            # QApplication 入口
│   ├── MainWindow.h/.cpp   # 主窗口，承载 GameWidget
│   ├── GameWidget.h/.cpp   # 游戏主控件，负责主循环、输入、绘制和游戏逻辑
│   └── ResourceManager.*   # 图片、字体和音效资源管理
├── build/                  # CMake 构建输出目录，不需要提交
└── .qtcreator/             # Qt Creator 用户配置目录，不需要提交
```

## 核心类说明

### `MainWindow`

`MainWindow` 继承自 `QMainWindow`，负责设置窗口标题、固定窗口大小，并将 `GameWidget` 设置为中心控件。

### `GameWidget`

`GameWidget` 是当前版本最核心的类，继承自 `QWidget`，主要负责：

- 使用 `QTimer` 以 60 FPS 驱动游戏刷新
- 在 `paintEvent()` 中使用 `QPainter` 绘制背景、恐龙、障碍物、火球、UI 和文字
- 在 `keyPressEvent()` / `keyReleaseEvent()` 中处理键盘输入
- 维护欢迎、运行、暂停、失败等游戏状态
- 维护分数、最高分、速度、障碍物生成计时器等全局游戏状态
- 通过内部结构体实现恐龙、背景层、障碍物和火球逻辑

当前第一版为了迁移效率，把主要游戏对象放在 `GameWidget` 内部实现。后续如果需要继续优化，可以再把 `PlayerDino`、`ProjectileManager`、`FireballManager`、`Hud` 等拆成独立类。

### `ResourceManager`

`ResourceManager` 负责加载和访问资源：

- 使用 `QPixmap` 加载 PNG 图片
- 使用 `QFontDatabase` 加载 `TEXTS.ttf`
- 使用 `QSoundEffect` 播放 WAV 音效
- 当图片加载失败时提供醒目的占位图，避免程序直接崩溃

## 操作说明

| 按键 | 行为 |
|---|---|
| `Space` / `Up` | 跳跃；欢迎界面时开始游戏 |
| `Down` | 地面冲刺；空中快速下落 |
| `Left` | 开启无敌模式，消耗全部体力 |
| `Right` | 发射火球，消耗 1 点体力 |
| `R` | 游戏结束后重新开始 |
| `Esc` | 暂停 / 继续 |

## SDL 到 Qt 的对应关系

迁移过程中，原 SDL 相关功能被替换为 Qt 原生能力：

| 原 SDL 实现 | QtDino 实现 |
|---|---|
| `SDL_Window` | `QMainWindow` / `QWidget` |
| `SDL_Renderer` | `QPainter` |
| `SDL_RenderCopy` / `SDL_RenderCopyEx` | `QPainter::drawPixmap()` 和旋转绘制 |
| `SDL_Texture` | `QPixmap` |
| `SDL_Rect` | `QRect` |
| `SDL_PollEvent` | `keyPressEvent()` / `keyReleaseEvent()` |
| `SDL_Delay` 控制帧率 | `QTimer` |
| `SDL_ttf` 字体渲染 | `QFontDatabase` + `QPainter::drawText()` |
| `IMG_LoadTexture` | `QPixmap` 加载 PNG |
| `PlaySound` / WAV | `QSoundEffect` |
| `malloc/free` 管理游戏对象 | C++ 对象、数组和 Qt 容器 |

## 已知状态与后续方向

当前版本的重点是完成第一版 Qt 迁移和玩法复现，因此暂时没有追求最终工程结构和视觉优化。

后续可以继续改进：

- 将 `GameWidget` 中的内部结构体拆成独立类
- 增加更清晰的游戏状态机
- 微调跳跃、冲刺、障碍物生成和碰撞手感
- 添加调试碰撞框开关
- 完善错误提示和资源加载日志
- 优化 UI 绘制位置和字体适配
- 增加更完整的测试和发布打包说明

目前建议先以这个版本作为 C/SDL 到 C++/Qt 的初步复现成果，再围绕课程要求继续补充说明、截图或代码分析。
