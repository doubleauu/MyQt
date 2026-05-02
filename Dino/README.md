# Runz's Dino 项目说明

这是一个使用 C 语言和 SDL2 编写的横版跑酷小游戏，玩法类似 Chrome Dino。项目使用 SDL2 负责窗口、渲染、输入和音频，游戏主体包含恐龙角色、背景滚动、障碍物、火球攻击、生命值、体力值、分数和暂停/重开逻辑。

## 1. 项目文件结构

```text
Dino/
├─ src/
│  ├─ CMakeLists.txt          # CMake 构建配置
│  ├─ GameFrame.c             # 程序入口、主循环、事件处理、全局游戏流程
│  ├─ Resource.c/.h           # 图片、字体、音频资源加载与释放
│  ├─ Background.c/.h         # 背景层对象与滚动逻辑
│  ├─ Dino.c/.h               # 玩家恐龙状态、移动、碰撞框、UI 显示
│  ├─ Projectile.c/.h         # 障碍物、飞鸟、能量球等移动对象
│  └─ Fireball.c/.h           # 玩家发射的火球
├─ bin/
│  ├─ Runz's Dino.exe         # 已编译可执行文件
│  ├─ SDL2.dll
│  ├─ SDL2_image.dll
│  ├─ SDL2_ttf.dll
│  └─ Resources/
│     ├─ TEXTS.ttf
│     ├─ Audios/              # GOD/HURT/JUMP/SCORE/SHOOT 音效
│     └─ Textures/            # 背景、恐龙、障碍物、UI、火球贴图
└─ 看我看我看我(课题报告精简版).pdf
```

## 2. main loop 在哪里

主入口在 `src/GameFrame.c` 的 `main()`。

程序有两层循环：

1. `main()` 中的外层 `while (1)`
   - 每次循环代表一局游戏。
   - 调用 `Var_Init()` 初始化变量。
   - 调用 `Resource_Load(Renderer)` 加载资源。
   - 调用 `Background_Init()` 初始化背景。
   - 调用 `Dino_Init()` 初始化玩家。
   - 调用 `Event_Loop()` 进入单局游戏循环。
   - 单局结束后调用 `DestroyEverything()` 释放本局资源。

2. `Event_Loop()` 中的内层 `while (1)`
   - 这是实际的帧循环。
   - 每帧更新背景、玩家、障碍物、火球、分数、UI。
   - 每帧处理 SDL 输入事件。
   - 每帧调用 `SDL_RenderPresent(Renderer)` 提交画面。
   - 使用 `SDL_Delay()` 控制帧率为 60 FPS。

## 3. SDL 初始化、窗口、渲染器在哪里

都在 `src/GameFrame.c` 的 `main()` 中。

```c
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

Window = SDL_CreateWindow(
    "Runz's Dino",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    SDL_WINDOW_SHOWN
);

Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

TTF_Init();
```

窗口大小由宏定义控制：

```c
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 800
```

退出时在 `QuitGame()` 中释放：

```c
SDL_DestroyWindow(Window);
SDL_DestroyRenderer(Renderer);
SDL_Quit();
```

## 4. 输入处理在哪里

输入处理在 `src/GameFrame.c` 的 `Event_Loop()` 中。

核心代码使用：

```c
SDL_Event event;
if (SDL_PollEvent(&event)) {
    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_QUIT:
    }
}
```

按键含义如下：

| 按键 | 行为 |
|---|---|
| `SPACE` / `UP` | 跳跃；欢迎界面时开始游戏 |
| `DOWN` | 冲刺/下蹲；空中时快速下落 |
| `LEFT` | 开启无敌模式 |
| `RIGHT` | 发射火球 |
| `R` | 游戏结束后重新开始 |
| `ESC` | 暂停/继续 |
| 关闭窗口 | 释放资源并退出 |

按键状态会先写入全局变量，例如 `SPACE_Pressed`、`DOWN_Pressed`、`Attack`，随后由 `Key_Intereact()` 进一步转换成玩家行为：

```c
if (DOWN_Pressed)
    Dino_Sprint();

if (SPACE_Pressed)
    Dino_Jump();
```

## 5. 游戏对象有哪些

### Dino

定义在 `Dino.h`。

玩家对象，包含是否冲刺、是否在空中、是否失败、是否受击闪烁、是否无敌、垂直速度、显示位置和碰撞框。对应全局实例在 `Dino.c`：

```c
Dino Player;
```

### Background

定义在 `Background.h`。

背景层对象，包含背景显示矩形、滚动速度倍率和背景纹理。游戏中有四层背景：

```c
Background *LAYER_GROUND;
Background *LAYER_CLOUD;
Background *LAYER_MOON;
Background *LAYER_STAR;
```

### Projectiles

定义在 `Projectile.h`。

用于表示障碍物和道具，包括飞鸟、大小仙人掌、混合仙人掌和能量球。主要字段包括纹理、位置、动画帧、碰撞框、速度、旋转角度、生命值、是否动画、是否能量球。

游戏中最多同时维护 10 个指针，但实际逻辑主要遍历前 5 个：

```c
Projectiles *Obstacle[10];
```

### Fireball

定义在 `Fireball.h`。

玩家攻击用的火球，只保存一个位置矩形：

```c
typedef struct {
    SDL_Rect POS;
} Fireball;
```

游戏中最多维护 5 个火球指针，但实际逻辑主要遍历前 3 个：

```c
Fireball *Fire[5];
```

### Resource

资源模块不是单个结构体，而是一组全局 SDL 资源，包括背景纹理、恐龙动作纹理、障碍物纹理、UI 纹理、字体和音频。

## 6. 每个 .c/.h 文件的作用

### `GameFrame.c`

项目核心文件。负责 `main()` 程序入口、SDL 初始化、窗口和渲染器创建、游戏主循环、每局初始化、输入事件处理、暂停、开始、结束、重开、分数和速度更新、障碍物生成、火球攻击逻辑、背景/玩家/UI 的调度渲染，以及退出和资源释放。

### `Resource.c`

资源加载和释放模块。负责加载字体、背景贴图、恐龙动作贴图、障碍物贴图、UI 贴图和部分 WAV 音效；同时负责渲染分数、欢迎、结束、暂停文字，并提供纹理访问函数。

### `Resource.h`

声明资源模块接口，包括 `Resource_Load()`、`Resource_Unload()`、`Print_Font()`、`Print_Font2()`、`Get_UI_Text()`、`Get_Layer_Text()`、`Get_Obstacle_Text()` 和 `Get_Dino_Text()`。

注意：`Resource.h` 声明了 `Get_Icon()`，但当前 `Resource.c` 中没有看到对应实现。

### `Background.c`

背景对象实现。负责创建背景层、根据速度滚动背景、背景滚出屏幕后重置位置，以及释放背景对象。

### `Background.h`

定义 `Background` 结构体和背景相关函数接口。

### `Dino.c`

玩家恐龙实现。负责初始化玩家状态、跳跃、冲刺/下蹲、取消冲刺、无敌模式、受击和生命值处理、体力值处理、重力和垂直运动、动画帧切换、玩家绘制，以及生命值/体力 UI 绘制。

### `Dino.h`

定义 `Dino` 结构体和玩家相关函数接口。

### `Projectile.c`

障碍物和能量球实现。负责创建 projectile 对象、更新位置/旋转角度/动画帧、检测是否撞到玩家、随机生成不同类型障碍物/道具、渲染障碍物和销毁 projectile。

### `Projectile.h`

定义 `Projectiles` 结构体和障碍物相关函数接口。

### `Fireball.c`

火球实现。负责创建火球、火球向右移动、火球与障碍物碰撞检测、渲染火球和销毁火球。

### `Fireball.h`

定义 `Fireball` 结构体和火球相关函数接口。

### `CMakeLists.txt`

CMake 构建配置。当前项目使用 C11，并链接：

```cmake
SDL2 SDL2_image SDL2_ttf winmm
```

## 7. 如果迁移到 C++/Qt Widgets，建议的类结构是什么

推荐将现在的全局变量和 C 风格模块拆成 C++ 类。

```text
MainWindow : QMainWindow
└─ GameWidget : QWidget
   ├─ GameController
   ├─ ResourceManager
   ├─ PlayerDino
   ├─ BackgroundLayer[]
   ├─ ProjectileManager
   │  └─ Projectile[]
   ├─ FireballManager
   │  └─ Fireball[]
   ├─ Hud
   └─ AudioManager
```

建议类职责如下：

| 类 | 建议职责 |
|---|---|
| `MainWindow` | Qt 主窗口，承载 `GameWidget` |
| `GameWidget` | 游戏画布，重写 `paintEvent()`、`keyPressEvent()`、`keyReleaseEvent()` |
| `GameController` | 游戏状态机、分数、速度、暂停、开始、结束、重开 |
| `ResourceManager` | 使用 `QPixmap`/`QImage`/`QFont` 管理资源 |
| `PlayerDino` | 玩家状态、跳跃、冲刺、受击、体力、生命、动画帧 |
| `BackgroundLayer` | 单层背景滚动 |
| `Projectile` | 单个障碍物/道具 |
| `ProjectileManager` | 障碍物生成、更新、碰撞、回收 |
| `Fireball` | 单个火球 |
| `FireballManager` | 火球创建、更新、碰撞、回收 |
| `Hud` | 绘制分数、最高分、生命、体力、提示文字 |
| `AudioManager` | 使用 `QSoundEffect` 或 `QMediaPlayer` 播放音效 |

Qt 版本中，原来的 SDL 主循环不应保留。建议使用 `QTimer` 驱动游戏帧：

```cpp
connect(timer, &QTimer::timeout, this, &GameWidget::tick);
timer->start(1000 / 60);
```

`tick()` 中执行：

```text
处理输入状态
更新游戏对象
碰撞检测
更新分数
调用 update() 触发 paintEvent()
```

## 8. 哪些部分可以直接复用逻辑，哪些必须重写

### 可以直接复用的逻辑

以下部分可以保留算法和数值设计，但需要改成 C++/Qt 类型：

| 原逻辑 | 可复用内容 |
|---|---|
| 恐龙跳跃 | 速度、重力、落地判断 |
| 恐龙冲刺 | 冲刺状态、碰撞框变化 |
| 恐龙受击 | 无敌/闪烁时间、生命减少 |
| 体力系统 | 发射火球消耗体力、能量球恢复体力 |
| 分数系统 | 根据速度增加分数，冲刺加分 |
| 难度提升 | 分数达到阈值后提高 `Setspeed` |
| 障碍物生成 | 随机生成障碍物类型和位置 |
| 碰撞检测 | 矩形 AABB 碰撞判断 |
| 火球攻击 | 火球移动、命中障碍物后移除 |
| 背景滚动 | 多层背景按不同倍率移动 |

### 必须重写的部分

以下部分与 SDL 强绑定，迁移到 Qt 时基本需要重写：

| SDL 现有实现 | Qt 替代方案 |
|---|---|
| `SDL_Init` / `SDL_Quit` | Qt 应用生命周期 `QApplication` |
| `SDL_Window` | `QMainWindow` / `QWidget` |
| `SDL_Renderer` | `QPainter` |
| `SDL_Texture` | `QPixmap` / `QImage` |
| `SDL_Rect` | `QRect` / `QRectF` |
| `SDL_RenderCopy` | `QPainter::drawPixmap` |
| `SDL_PollEvent` | `keyPressEvent` / `keyReleaseEvent` |
| `SDL_Delay` 控制帧率 | `QTimer` |
| `SDL_ttf` 字体渲染 | `QPainter::drawText` / `QFont` |
| `IMG_LoadTexture` | `QPixmap::load` |
| `PlaySound` / `SDL_LoadWAV` | `QSoundEffect` / `QMediaPlayer` |
| 手动 `malloc/free` | C++ 对象、智能指针、容器 |

### 建议迁移策略

第一步先不要急着改 UI，而是提取纯逻辑：

- 把 `Dino` 改成 `PlayerDino` 类
- 把 `Projectiles` 改成 `Projectile` 类
- 把 `Fireball` 改成 `Fireball` 类
- 把 `Background` 改成 `BackgroundLayer` 类
- 把 `SDL_Rect` 替换为 `QRect`

第二步再替换渲染：

- 资源从 `SDL_Texture` 改为 `QPixmap`
- 绘制从 `SDL_RenderCopy` 改为 `QPainter`
- 文字从 `SDL_ttf` 改为 `QPainter::drawText`

第三步重构主循环：

- 删除 `Event_Loop()`
- 用 `QTimer` 实现固定帧率
- 用 `keyPressEvent()` / `keyReleaseEvent()` 维护输入状态
- 用 `paintEvent()` 统一绘制

最终 Qt Widgets 版本应该从“过程式全局状态”转成“对象持有状态 + GameWidget 驱动更新”的结构。
