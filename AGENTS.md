# AGENTS.md - 项目记忆

## 项目定位

这个仓库用于学习、复现和迁移一个类 Chrome Dino 的横版跑酷小游戏。仓库中同时保留了原始 C/SDL2 版本、已经完成第一版迁移的 Qt Widgets 版本，以及一个更适合继续手写练习的 Qt 骨架版本。

当前最完整、最值得优先参考和运行的是 `QtDino/`。`Dino/` 是原始参考实现，`MyDino/` 目前还只是学习用最小 Qt 工程骨架。

## 顶层目录

- `Dino/`
  - 原始 C/SDL2 项目。
  - `src/` 包含完整 C 源码。
  - `bin/Resources/` 保留了原项目资源，包含字体、贴图和 WAV 音效。
  - `看我看我看我(课题报告精简版).pdf` 是两页中文技术文档，说明原版开发环境、玩法、按键和机制。
- `QtDino/`
  - 从 `Dino/` 迁移来的 C++17/Qt 6 Widgets 版本。
  - 玩法、资源、窗口尺寸和大部分数值都尽量复刻原 SDL 版。
  - 已有 `ResourceManager`，用 Qt 的 `QPixmap`、`QFontDatabase`、`QSoundEffect` 替换 SDL 资源和音效管理。
- `MyDino/`
  - 自己练习用 Qt 版本骨架。
- `.gitignore`
  - 忽略构建产物、IDE 文件、可执行文件和 DLL。
  - 明确保留 `Dino/bin/Resources/**`，因为这些资源是项目资料的一部分。

## 原始 C/SDL2 版本记忆

原始入口在 `Dino/src/GameFrame.c` 的 `main()`。程序创建 `SDL_Window`、`SDL_Renderer`，初始化 `TTF`，然后在外层 `while (1)` 中反复开新局：初始化变量、加载资源、初始化背景和恐龙，进入 `Event_Loop()`，结束后释放资源。

窗口和帧率：

- `WINDOW_WIDTH = 1600`
- `WINDOW_HEIGHT = 800`
- `FRAMERATE = 60`

核心全局状态：

- 背景：`LAYER_GROUND`、`LAYER_CLOUD`、`LAYER_MOON`、`LAYER_STAR`
- 玩家：`Dino Player`
- 障碍物数组：`Projectiles *Obstacle[10]`，实际主要遍历前 5 个
- 火球数组：`Fireball *Fire[5]`，实际主要遍历前 3 个
- 游戏状态：`Gamewelcome`、`Gameover`、`Gamepause`
- 输入状态：`SPACE_Pressed`、`DOWN_Pressed`、`RESTART_Pressed`、`Attack`
- 分数：`Score`、`HighScore`
- 速度：`Setspeed`

主要源码职责：

- `GameFrame.c`
  - SDL 生命周期、主循环、输入事件、暂停/失败/重开、分数、速度、障碍生成、火球攻击、背景/玩家/UI 调度。
- `Resource.c/.h`
  - 加载字体、背景、恐龙动作、障碍物、UI、火球和部分 WAV。
  - `Resource.h` 声明了 `Get_Icon()`，但 `Resource.c` 中没有对应实现。
- `Background.c/.h`
  - 保存单层背景的目标矩形、速度倍率和贴图，按不同倍率滚动，移动到 `x <= -1600` 后复位。
- `Dino.c/.h`
  - 玩家状态、跳跃、冲刺/快速下落、无敌、受击闪烁、生命、体力、动作帧、碰撞框和 UI。
  - 重要数值：地面高度 500，跳跃速度 1840，下落速度 -1840，重力 80，生命 3，体力 3。
- `Projectile.c/.h`
  - 鸟、仙人掌、能量球的生成、移动、动画、旋转、碰撞与销毁。
  - 鸟在 `y = rand() % 250 + 200`，能量球在 `y = rand() % 300 + 150`。
- `Fireball.c/.h`
  - 火球从 `x = 220`、玩家嘴部高度 `y + 20` 发出，每帧向右移动 30，与障碍 AABB 碰撞后移除。

原始按键：

- `Space` / `Up`：跳跃；欢迎界面开始游戏
- `Down`：地面冲刺；空中快速下落
- `Left`：仙人模式/无敌，消耗 3 格体力
- `Right`：发射火球，消耗 1 格体力
- `R`：失败后重开
- `Esc`：暂停/继续
- 关闭窗口：释放资源并退出

原始机制：

- 分数每帧按 `Setspeed - 1` 增长。
- 玩家在地面冲刺时额外加分。
- 分数约到 3000 时速度升到 3，约到 6000 时速度升到 4。
- 障碍生成由 `Timer_Obstacle > (180 / Setspeed)` 控制，再以 30% 概率生成随机障碍或能量球。
- 能量球不会扣血，命中玩家后恢复 1 格体力，上限 3。
- 非能量障碍命中玩家时，如果玩家没有受击保护且不在无敌状态，会扣 1 生命并进入闪烁保护。
- 生命归零后进入失败状态，恐龙显示 `Fail.png`。

## QtDino 迁移版记忆

`QtDino/` 是完整 Qt 迁移版，CMake 项目名为 `QtDino`，使用：

- C++17
- Qt 6 Widgets
- Qt Multimedia
- CMake 3.24+

入口和窗口：

- `src/main.cpp` 创建 `QApplication`，显示 `MainWindow`。
- `src/MainWindow.cpp` 设置标题 `Runz's Dino`，固定窗口 `1600 x 800`，中央控件是 `GameWidget`。

资源管理：

- `src/ResourceManager.h/.cpp`
  - `load()` 加载 `Resources/TEXTS.ttf`、所有 PNG 贴图、五个 WAV 音效。
  - 贴图通过名字访问，例如 `run1`、`layerGround`、`cactusBig1`、`stamia3`。
  - 音效通过名字播放：`god`、`hurt`、`jump`、`score`、`shoot`。
  - 加载失败时用 32x32 洋红色 `missingTexture_` 兜底。
  - `resourcesRoot()` 优先找可执行文件旁的 `Resources`，其次找 `../Resources`，最后找当前目录下 `Resources`。

核心控件：

- `src/GameWidget.h/.cpp`
  - 继承 `QWidget`。
  - 构造函数固定窗口大小、设置强焦点、加载资源、重置局面，启动 60 FPS `QTimer`。
  - `paintEvent()` 用 `QPainter` 绘制背景、玩家、火球、障碍、HUD、欢迎/暂停/失败文字、分数和最高分。
  - `keyPressEvent()` / `keyReleaseEvent()` 维护输入状态和暂停/重开逻辑。
  - `tick()` 是每帧更新入口。

`GameWidget` 内部结构：

- `BackgroundLayer`
  - 保存贴图名、目标矩形和速度倍率。
- `PlayerDino`
  - 基本等价于原版 `Dino` 全局状态，但封装进结构体。
  - 处理 `reset()`、`jump()`、`sprintDown()`、`deSprint()`、`setGodMode()`、`hit()`、`alive()`、`currentMotion()`、`updatePosition()` 等。
- `Projectile`
  - 用于鸟、仙人掌、能量球。
  - 字段包括 `active`、`animated`、`energy`、`texture`、`pos`、`frame`、`collisionBox`、速度和角度。
- `Fireball`
  - 只保存 `active` 和 `QRect pos`。

Qt 迁移对应关系：

- `SDL_Window` -> `QMainWindow` / `QWidget`
- `SDL_Renderer` -> `QPainter`
- `SDL_Texture` -> `QPixmap`
- `SDL_Rect` -> `QRect`
- `SDL_PollEvent` -> `keyPressEvent()` / `keyReleaseEvent()`
- `SDL_Delay` 控帧 -> `QTimer`
- `SDL_ttf` -> `QFontDatabase` + `QPainter::drawText()`
- `PlaySound` / WAV -> `QSoundEffect`
- `malloc/free` -> 值对象、数组和 Qt 容器

QtDino 运行/构建方式：

推荐用 Qt Creator 打开：

```text
C:\workspace\Qt\QtDino\CMakeLists.txt
```

已知环境记录：

```text
Qt: C:\Qt\6.5.3\mingw_64
Kit: Desktop Qt 6.5.3 MinGW 64-bit
CMake: C:\Qt\Tools\CMake_64\bin\cmake.exe
Ninja: C:\Qt\Tools\Ninja\ninja.exe
MinGW: C:\Qt\Tools\mingw1120_64
g++: 11.2.0
```

命令行构建参考：

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

```powershell
C:\Qt\Tools\CMake_64\bin\cmake.exe --build QtDino/build
```

`QtDino/CMakeLists.txt` 有 post-build 步骤，会把 `Resources/` 复制到可执行文件目录，并在 Windows 下调用 `windeployqt --no-translations --no-compiler-runtime` 部署运行依赖。

## MyDino 状态追踪

`MyDino/` 是用户自己手写复现的 Qt Widgets 练习工程。状态记录按时间追加，不覆盖旧记录。

### 2026-05-04 19:33

- 已完成 `GameWidget` 游戏画布、60 FPS `QTimer` 游戏循环、`QPainter` 基础绘制、键盘跳跃、基础地面滚动、资源目录复制配置、`Run1.png` 恐龙图片加载和绘制。
- `CMakeLists.txt` 当前依赖 `Qt6::Widgets`，并通过 `add_custom_command(... copy_directory ...)` 在构建后复制 `MyDino/Resources` 到可执行文件旁边。
- `MainWindow.cpp` 已承载 `GameWidget`，窗口为 `1600 x 800`，标题为 `MyDino`。
- `GameWidget` 当前使用 `QRect dinoRect_ {20, 500, 200, 200}` 绘制恐龙；地面线在 `y = 700`，恐龙尺寸和站立位置已基本对齐 `QtDino`。
- 当前跳跃参数已对齐 `QtDino`：`GroundY = 500`、`JumpVelocity = 1840`、`Gravity = 80`，并通过 `delta = velocityY_ / 60` 计算每帧位移；按键支持 `Space`、`Up`、`W` 起跳。
- 当前资源接入方式：`GameWidget` 使用 `QCoreApplication::applicationDirPath() + "/Resources/Textures/Run1.png"` 加载恐龙图片。

### 2026-05-04 20:23

- 已完成并推送 `实现恐龙跑步和跳跃动画`。
- `GameWidget` 已从单张 `Run1.png` 绘制改为加载 `Run1.png`、`Run2.png`、`Idle.png`。
- 地面状态下通过计数器在 `Run1` / `Run2` 之间切换，形成跑步动画。
- 跳跃状态下显示 `Idle.png`，落地后恢复跑步动画。
- 对应提交：`5e2a90e`，提交信息：`实现恐龙跑步和跳跃动画`。

### 2026-05-04 21:09

- 已完成 `实现障碍物移动和碰撞检测`。
- `GameWidget` 已加入 `Cactus_SMALL1.png` 仙人掌障碍物资源加载和绘制。
- 障碍物从窗口右侧向左移动，离开左侧后回到窗口右侧循环出现。
- 已加入恐龙碰撞框与障碍物矩形的 `QRect::intersects()` 检测。
- 碰撞后进入 `gameOver_` 状态，停止更新并在画面中央显示 `Game Over`。
- 本次提交信息：`实现障碍物移动和碰撞检测`。

### 2026-05-04 21:25

- 已完成 `实现游戏失败后重新开始`。
- `GameWidget` 已新增 `resetGame()`，用于重置恐龙位置、竖直速度、空中状态、障碍物位置、背景偏移、跑步动画帧和 `gameOver_`。
- 失败提示已改为 `Game Over\nPress R to Restart`。
- `keyPressEvent()` 已支持在 `gameOver_` 状态下按 `R` 重新开始；失败状态下除 `R` 外不处理其他游戏输入。
- 本次提交信息：`实现游戏失败后重新开始`。

如果后续要在 `MyDino/` 里继续手写复现，建议从 `QtDino/` 中逐步搬迁思路，而不是一次性复制整份实现；每次讲解前先只读检查用户当前代码进度。

## 资源记忆

主要资源位于：

- `QtDino/Resources/`
- `Dino/bin/Resources/`

两处资源基本一致：

- 字体：`TEXTS.ttf`
- 音效：`GOD.wav`、`HURT.wav`、`JUMP.wav`、`SCORE.wav`、`SHOOT.wav`
- 背景：`LAYER_GROUND.png`、`LAYER_CLOUD.png`、`LAYER_MOON.png`、`LAYER_STAR.png`
- 玩家：`Run1.png`、`Run2.png`、`Sprint1.png`、`Sprint2.png`、`Idle.png`、`Fail.png`
- 障碍/道具：`Bird1.png`、`Bird2.png`、各种 `Cactus_*.png`、`Energyball.png`、`Fireball.png`
- UI：`Heart1/2/3.png`、`Stamia0/1/2/3.png`

注意资源命名中使用了 `Stamia`，这是项目现有拼写。维护时尽量沿用，避免和已有代码的资源键不一致。

## PDF 报告摘记

`Dino/看我看我看我(课题报告精简版).pdf` 共 2 页。主要信息：

- 标题：`Runz's Dino 技术文档`
- 原始平台：Windows 10 Professional 21H2
- C 环境：MinGW gcc 11.2.0
- SDL 依赖：SDL2 2.26.x、SDL2_image 2.6.2、SDL2_ttf 2.20.1
- 运行原版：`/bin/Runz's Dino.exe`
- 分辨率：`1600 * 800`
- HighScore 退出后不会持久化。
- 操作和机制与 README/source 一致：跳跃、冲刺/快速下落、仙人模式、火球、重开、暂停；体力用于火球和无敌，能量球恢复体力，生命为三颗心，冲刺加分，分数越高速度越快。

## 维护提醒

- 不要删除 `Dino/bin/Resources/**`，`.gitignore` 特意保留它们。
- 不要把 build 目录、Qt Creator 用户配置、可执行文件、DLL 提交进版本库。
- `QtDino` 是当前完成度最高的实现；修玩法问题优先看 `QtDino/src/GameWidget.cpp`。
- `GameWidget` 目前仍是集中式实现，后续重构可以拆成 `PlayerDino`、`BackgroundLayer`、`ProjectileManager`、`FireballManager`、`Hud`、`AudioManager` 等独立类。
- `ResourceManager::loadTexture()` 和 `loadSound()` 每次会调用 `resourcesRoot()`；功能没问题，但如果后续资源变多，可以缓存 root。
- `QSoundEffect` 适合短音效；如果后续加背景音乐，可另引入 `QMediaPlayer`。
- 原 C 版有一些手动内存管理和潜在悬空指针风险，Qt 版已经通过值对象/数组规避了大部分问题。
- 分数和速度阈值目前沿用原版的近似窗口判断：`score > 3000 && score < 3010`、`score > 6000 && score < 6010`。
- 如果调整窗口尺寸，背景图、HUD、玩家坐标、碰撞框、中心文字和障碍生成坐标都需要一起改。
- 如果修文字显示，留意当前 README 在某些控制台编码下可能显示乱码，但文件本身记录的是中文项目说明。

## 当前阅读结论

这个仓库的主线是：以 `Dino/` 的 C/SDL2 版为原始样本，已经在 `QtDino/` 中完成了一版可构建、可运行、可初步游玩的 C++/Qt 迁移；`MyDino/` 则保留为继续练习和逐步复现的空间。后续开发如果追求快速可玩，应改 `QtDino/`；如果追求学习过程，应在 `MyDino/` 中按模块逐步复刻。

## MyDino 操作规则

- `MyDino/` 是用户自己动手复现和学习 Qt/CMake 的练习项目。除非用户明确要求修改某个文件、撤回某个改动或提交某次已验收成果，否则不要对 `MyDino/` 执行任何写入、构建、运行、格式化、提交或推送操作。
- 默认只提供讲解、步骤、示例代码、排错思路和验收清单，让用户自己在 `MyDino/` 中实现。
- 当用户询问 `MyDino/` 复刻步骤时，每一步都要同时给出必要代码和简要解释，解释代码的作用、为什么这样写，以及该步骤的验收点。
- 指导用户复刻 `MyDino/` 时，流程可以为了学习而简化，但涉及画面比例、坐标、速度、重力、碰撞框、资源尺寸、窗口大小等会影响后续功能的关键参数，应优先参考并对齐 `QtDino/` 的现有实现，避免用户后续返工。
- 每次进行 `MyDino/` 复刻讲解前，先只读检查用户当前 `MyDino/` 代码和实现进度，再给出下一步说明，避免脱离实际代码的模糊回答。
- 更新 `MyDino/` 状态时，只能在 `MyDino 状态追踪` 中追加新的时间记录，不要覆盖或删除旧状态记录，除非用户明确要求整理历史。
- 每次提交 `MyDino/` 复刻相关成果前，先更新 `AGENTS.md` 中的 `MyDino 状态追踪`，追加本次完成内容、关键实现点和对应提交信息，再进行提交。
