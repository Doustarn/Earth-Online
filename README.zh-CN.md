# Immortal Dao Online · 修仙 Online

[English (GitHub)](https://github.com/Doustarn/Immortal-Dao-Online)

一款基于 **Unreal Engine 5.8** 的大型修仙开放世界 MMORPG。
无固定剧情，无强制任务——你从一介散修起步，炼气、筑基、结丹、元婴……一路证道超脱。

> 由 **Doustar / 逗星科技** 开发 · 当前版本 **v0.1.8.1.1001**（Alpha 内测）

---

## ✨ 特色

- **21 大境界 × 每境 10 级** —— 炼气 → 筑基 → 金丹 → 元婴 → 化神 → 合体 → 渡劫 → 大乘 → 渡仙劫 → 散仙 → 地仙 → 天仙 → 金仙 → 太乙金仙 → 大罗金仙 → 准圣 → 亚圣 → 圣人 → 不朽圣人 → 天道境 → 超脱境 → 无境
- **随机接替 NPC 开局** —— 世界全 NPC 自动运转，玩家注册后随机接替一个 NPC 身份（宗门 + 道号 + 境界），修为不超过化神
- **5 大职业** —— 剑修 / 枪修 / 法修 / 丹修 / 符修，各有独立技能树
- **宗门系统** —— 金丹期可开山立派，建大殿、功法阁、丹药阁、护山大阵，招募 NPC 弟子，宗门间结盟宣战夺矿脉
- **修为货币分层** —— 灵石(4阶) → 神石 → 仙晶 → 天道石 → 功德 → 气运 → 证道
- **离线掉段** —— 每年未登录，低段位（炼气~化神）掉 3 个大境界，高段位（合体+）掉 2 个，保底不低于炼气期
- **双语支持** —— 注册时按 IP 自动选择：华人区简体中文，其他地区 English；游戏内可随时切换
- **大世界练级** —— 野外山贼、妖兽、邪修轮番出没，击杀掉修为经验与灵石

## 🛠 技术栈

| 层 | 选择 |
|----|------|
| 引擎 | Unreal Engine 5.8.1 (DX12, SM6) |
| 语言 | C++ + UMG（动态构建 UI） |
| 网络 | UE 内置复制 (NetServerMaxTickRate = 60) |
| 双语 | 静态语言管理类 + 注册时选择 |

## 📂 仓库结构

```
Immortal-Dao-Online/
├── UE5/
│   ├── Config/            # 引擎/输入/游戏配置
│   ├── Content/           # 地图/UI 贴图/材质
│   ├── Source/EarthOnline/
│   │   ├── Character/         # 玩家角色
│   │   ├── GameInstance/      # 网络/服务器
│   │   ├── GameMode/          # 游戏模式（怪物生成+掉段）
│   │   ├── HUD/               # UI 流程
│   │   ├── PlayerController/  # 输入/交互
│   │   ├── Utils/             # 双语 EOLang
│   │   └── UI/                # 主菜单/登录/HUD/职业选择
│   └── EarthOnline.uproject
└── LICENSE                # AGPLv3
```

## 🚀 快速开始

1. 安装 Unreal Engine **5.8.1**
2. Clone 本仓库
3. 右键 `EarthOnline.uproject` → 生成 VS 工程文件
4. 打开 `.sln`，编译 Development Editor，按 Play

## 🚀 自行打包

自己打包独立 Windows 客户端：

1. 从 Epic Games Launcher 安装 **Unreal Engine 5.8.1**
2. Clone 本仓库，用 UE 编辑器打开 `EarthOnline.uproject`
3. 等着色器编译完，点 **文件 → 打包项目 → Windows → Windows (64位)**
4. 选输出目录（比如 `UE5/Packaged/`）
5. 打包完成后从输出目录运行 `EarthOnline.exe`

> **注意**：需要安装 Visual Studio 2022 并勾选"使用 C++ 的游戏开发"工作负载才能编译 C++ 源码。

## 📦 v0.1.8.1.1001 (Alpha) — 本次内容

首个 Alpha 版本包含：

- **青色主题主菜单**，带语言选择（简体中文 / English）
- **登录流程**：输入道号 → 随机 NPC 身份揭示 → 选择修仙之路（5 职业：剑修/枪修/法修/丹修/符修）
- **游戏内 HUD**：左下角修为境界+等级+经验条+灵石，右下角 Q/W/E/R 技能栏，9 格快捷栏
- **野外敌人**：玩家周围随机生成 8 个山贼/妖兽/邪修
- **离线掉段系统**：1 年未登录掉境界（低段位-3，高段位-2，保底炼气期）
- **双语 UI**：简体中文或 English，主菜单可切换

已知问题：暂无战斗系统，敌人是纯视觉方块，无存档，无联网。

## 📦 发行版

### v0.1.8.1.1001 (Alpha)

- 开始界面：星空背景、青色主题、中/英语言切换
- 登录流程：输入道号 → 随机前世身份 → 选择 5 大职业（剑修/枪修/法修/丹修/符修）
- 游戏内 HUD：境界栏（进入时随机生成）、Q/W/E/R 技能栏、9 格快捷栏
- 大世界：玩家周围刷新 8 个怪物（山贼/妖兽/邪修）
- 离线掉段：每年未登录低段位掉 3 个大境界，高段位掉 2 个

### 自行打包

1. 从 Epic Games Launcher 安装 **Unreal Engine 5.8.1**
2. Clone 本仓库
3. 右键 `EarthOnline.uproject` → **生成 Visual Studio 工程文件**
4. 打开 `EarthOnline.sln`，配置选 **Development Editor**，编译
5. 在编辑器里按 **Play** 运行

> **Hackm 提示**：现为内测阶段，Bug 非常之多。对于 Bug，Hackm 非常感谢有用的反馈！

## 📡 镜像

| 平台 | 地址 |
|------|------|
| GitHub | https://github.com/Doustarn/Immortal-Dao-Online |
| Gitee  | https://gitee.com/Doustar/Immortal-Dao-Online |

## 📄 许可证

**Immortal Dao Online** 是自由软件：你可以在遵守
**GNU Affero General Public License v3.0** 条款的前提下自由分发和修改。

详见 [LICENSE](./LICENSE)。
