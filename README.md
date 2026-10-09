# 地球Online (Earth Online)

大型拟真沙盒 MMO 游戏项目，基于 Unreal Engine 5 开发。

## 当前阶段：测试地图 + 光效渲染 + 材质验证

本阶段目标：搭建测试场景，验证 Lumen 全局光照、光线追踪反射、瓷砖/大理石反光材质效果。

---

## 目录结构

```
地球online/
├── UE5/                          # UE5 项目根目录
│   ├── EarthOnline.uproject      # 项目文件
│   ├── Config/                   # 引擎配置
│   │   └── DefaultEngine.ini     # 渲染、光照、物理配置
│   └── Content/                  # 游戏内容
│       ├── Maps/                 # 关卡地图
│       ├── Materials/            # 材质文件
│       ├── Textures/             # 纹理贴图（已生成8张PNG）
│       └── Meshes/               # 模型文件
├── 配置/                          # 配置文档
│   ├── 测试地图搭建指南.md        # 光照、后期处理、反射配置
│   └── 瓷砖与大理石材质配置.md    # 材质节点、参数、Substrate设置
└── 过程文件/                      # 开发过程文件
    ├── generate_textures.py       # 程序纹理生成脚本
    └── textures/                  # 生成的纹理源文件
```

---

## 快速开始

### 1. 打开项目
1. 安装 **Unreal Engine 5.3** 或更高版本
2. 双击 `UE5/EarthOnline.uproject` 打开项目
3. 首次打开会编译 Shader，等待完成

### 2. 导入纹理
1. 在 Content Browser 中打开 `Content/Textures`
2. 将 PNG 文件拖入（或使用 Import 按钮）
3. 导入后设置纹理属性：
   - `*_BC.png` → sRGB = 勾选（Color 空间）
   - `*_N.png` → Compression Settings = Normalmap
   - `*_R.png` → sRGB = 取消勾选（Linear 空间）
   - `*_AO.png` → sRGB = 取消勾选（Linear 空间）
   - `*_Vein_Mask.png` → sRGB = 取消勾选（Linear 空间）

### 3. 创建材质
参考 `配置/瓷砖与大理石材质配置.md`，按文档中的节点连接方式创建：
- `M_Tile_Glossy` - 亮面瓷砖材质
- `M_Marble_Polished` - 抛光大理石材质

### 4. 搭建测试地图
参考 `配置/测试地图搭建指南.md`：
1. 创建空关卡 `TestMap`
2. 放置定向光、天光、天空大气、指数高度雾
3. 放置 Post Process Volume（Lumen + 光追反射）
4. 放置地面 Plane，赋予瓷砖/大理石材质
5. 放置测试物体（球体、立方体等）
6. 运行验证光照和反射效果

---

## 已生成的纹理贴图

### 瓷砖纹理（亮面）
| 文件 | 用途 | 说明 |
|------|------|------|
| T_Tile_Glossy_BC.png | 基础色 | 4x4白色瓷砖 + 深色填缝 |
| T_Tile_Glossy_N.png | 法线 | 瓷砖表面微凹凸 + 填缝凹陷 |
| T_Tile_Glossy_R.png | 粗糙度 | 瓷砖面光滑(~0.1) + 填缝粗糙(~0.8) |
| T_Tile_Glossy_AO.png | 环境光遮蔽 | 填缝处AO遮蔽 |

### 大理石纹理（抛光）
| 文件 | 用途 | 说明 |
|------|------|------|
| T_Marble_Polished_BC.png | 基础色 | 米白底色 + 天然纹理脉络 |
| T_Marble_Polished_N.png | 法线 | 细微表面凹凸 + 脉络微凹陷 |
| T_Marble_Polished_R.png | 粗糙度 | 抛光面光滑(~0.08) + 脉络微粗糙 |
| T_Marble_Vein_Mask.png | 脉络遮罩 | 大理石纹理分布遮罩 |

所有纹理分辨率：**1024x1024**，可无缝平铺。

---

## 渲染配置要点

项目已在 `DefaultEngine.ini` 中预设：

- **Lumen 全局光照**：已启用（Diffuse + Specular + Reflections）
- **硬件光线追踪**：已启用（反射、阴影、AO、GI）
- **Substrate 材质框架**：已启用（支持清漆层、多层材质）
- **体积云/体积雾**：已启用
- **TAA 抗锯齿**：已启用，质量等级2
- **DX12**：默认 RHI

验证命令（控制台输入）：
```
r.Lumen.DiffuseIndirect.Allow 1
r.Lumen.Reflections.Allow 1
r.RayTracing.Reflections 1
stat unit
stat lumen
```

---

## 材质核心特性

### 亮面瓷砖
- Substrate 清漆层（Clear Coat）模拟釉面
- 清漆粗糙度 0.03-0.08，产生镜面反射
- 填缝与瓷砖面粗糙度差异明显
- 法线贴图控制填缝凹陷

### 抛光大理石
- 天然石材纹理脉络（程序噪声生成）
- 高抛光表面，近乎镜面反射
- 脉络处微粗糙度变化
- IOR 1.55，真实菲涅尔效应

---

## 后续计划

- [ ] 哑光瓷砖材质（M_Tile_Matte）
- [ ] 仿古大理石材质（M_Marble_Honed）
- [ ] 更多纹理变体（不同颜色、尺寸）
- [ ] 材质实例库（MI_*）
- [ ] 可交互测试场景
- [ ] 性能优化与基准测试
- [ ] 星球地图模块（后续阶段）
- [ ] 玩法系统（后续阶段）

---

## 技术栈

- **引擎**：Unreal Engine 5.3+
- **渲染**：Lumen + Hardware Ray Tracing + Substrate
- **材质**：PBR 物理材质，清漆层，各向异性
- **纹理**：程序化生成（Python + PIL），1024x1024
- **目标平台**：PC (DX12)

---

## 相关文档

- [测试地图搭建指南](配置/测试地图搭建指南.md)
- [瓷砖与大理石材质配置](配置/瓷砖与大理石材质配置.md)
- [纹理生成脚本](过程文件/generate_textures.py)
