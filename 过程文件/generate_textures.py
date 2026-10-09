"""
地球Online - 程序纹理生成器
生成瓷砖和大理石材质所需的全套纹理贴图
"""
import math
import random
from PIL import Image, ImageDraw, ImageFilter

# 输出目录
OUTPUT_DIR = r"D:\孟思辰\地球online\过程文件\textures"
import os
os.makedirs(OUTPUT_DIR, exist_ok=True)

# 纹理尺寸
SIZE = 1024

# ============================================================
# 工具函数
# ============================================================

def clamp(value, min_val=0, max_val=255):
    return max(min_val, min(max_val, int(value)))

def lerp(a, b, t):
    return a + (b - a) * t

def smooth_noise(x, y, seed=0):
    """简单的平滑噪声函数"""
    random.seed(seed + x * 1000 + y)
    return random.random()

def value_noise(x, y, scale=10, seed=0):
    """值噪声"""
    xi = int(x / scale)
    yi = int(y / scale)
    xf = (x / scale) - xi
    yf = (y / scale) - yi
    
    # 平滑插值
    u = xf * xf * (3 - 2 * xf)
    v = yf * yf * (3 - 2 * yf)
    
    n00 = smooth_noise(xi, yi, seed)
    n10 = smooth_noise(xi + 1, yi, seed)
    n01 = smooth_noise(xi, yi + 1, seed)
    n11 = smooth_noise(xi + 1, yi + 1, seed)
    
    nx0 = lerp(n00, n10, u)
    nx1 = lerp(n01, n11, u)
    return lerp(nx0, nx1, v)

def fbm(x, y, octaves=5, seed=0):
    """分形布朗运动噪声"""
    value = 0
    amplitude = 0.5
    frequency = 1
    for i in range(octaves):
        value += amplitude * value_noise(x * frequency, y * frequency, scale=20, seed=seed + i * 100)
        amplitude *= 0.5
        frequency *= 2
    return value

def normal_from_height(height_map, strength=1.0):
    """从高度图生成法线贴图"""
    w, h = height_map.size
    normal = Image.new("RGB", (w, h), (128, 128, 255))
    height_pixels = height_map.load()
    normal_pixels = normal.load()
    
    # 判断是否为灰度图
    is_grayscale = (height_map.mode == "L")
    
    for y in range(h):
        for x in range(w):
            # 采样周围高度
            if is_grayscale:
                hl = height_pixels[(x - 1) % w, y]
                hr = height_pixels[(x + 1) % w, y]
                hu = height_pixels[x, (y - 1) % h]
                hd = height_pixels[x, (y + 1) % h]
            else:
                hl = height_pixels[(x - 1) % w, y][0]
                hr = height_pixels[(x + 1) % w, y][0]
                hu = height_pixels[x, (y - 1) % h][0]
                hd = height_pixels[x, (y + 1) % h][0]
            
            # 计算法线
            dx = (hr - hl) * strength / 255.0
            dy = (hd - hu) * strength / 255.0
            dz = 1.0
            
            # 归一化
            length = math.sqrt(dx * dx + dy * dy + dz * dz)
            nx = dx / length
            ny = dy / length
            nz = dz / length
            
            # 转换到0-255
            normal_pixels[x, y] = (
                clamp((nx + 1) * 127.5),
                clamp((ny + 1) * 127.5),
                clamp((nz + 1) * 127.5)
            )
    return normal

# ============================================================
# 瓷砖纹理生成
# ============================================================

def generate_tile_textures():
    """生成亮面瓷砖全套纹理"""
    print("生成瓷砖纹理...")
    
    # 瓷砖参数
    tile_count = 4  # 4x4瓷砖
    tile_size = SIZE // tile_count
    gap_width = 8  # 填缝宽度（像素）
    
    # 1. 基础色贴图
    bc = Image.new("RGB", (SIZE, SIZE))
    bc_pixels = bc.load()
    
    # 瓷砖颜色（微变化的白色）
    tile_colors = []
    for i in range(tile_count * tile_count):
        variation = random.randint(-10, 10)
        tile_colors.append((
            clamp(240 + variation),
            clamp(238 + variation),
            clamp(235 + variation)
        ))
    
    for y in range(SIZE):
        for x in range(SIZE):
            tile_x = x // tile_size
            tile_y = y // tile_size
            local_x = x % tile_size
            local_y = y % tile_size
            
            # 检查是否在填缝处
            in_gap = (local_x < gap_width or local_x > tile_size - gap_width or
                      local_y < gap_width or local_y > tile_size - gap_width)
            
            if in_gap:
                # 填缝颜色（深灰色）
                gap_variation = random.randint(-5, 5)
                bc_pixels[x, y] = (
                    clamp(80 + gap_variation),
                    clamp(78 + gap_variation),
                    clamp(75 + gap_variation)
                )
            else:
                # 瓷砖表面（微噪声变化）
                noise = value_noise(x, y, scale=5, seed=42) * 10 - 5
                idx = tile_y * tile_count + tile_x
                base = tile_colors[idx]
                bc_pixels[x, y] = (
                    clamp(base[0] + noise),
                    clamp(base[1] + noise),
                    clamp(base[2] + noise)
                )
    
    bc.save(os.path.join(OUTPUT_DIR, "T_Tile_Glossy_BC.png"))
    print("  基础色贴图完成")
    
    # 2. 高度图（用于生成法线）
    height = Image.new("L", (SIZE, SIZE))
    h_pixels = height.load()
    
    for y in range(SIZE):
        for x in range(SIZE):
            local_x = x % tile_size
            local_y = y % tile_size
            
            in_gap = (local_x < gap_width or local_x > tile_size - gap_width or
                      local_y < gap_width or local_y > tile_size - gap_width)
            
            if in_gap:
                # 填缝处凹陷
                h_pixels[x, y] = 80
            else:
                # 瓷砖表面微凹凸
                noise = value_noise(x, y, scale=3, seed=123) * 20
                h_pixels[x, y] = clamp(200 + noise)
    
    # 3. 法线贴图
    normal = normal_from_height(height, strength=2.0)
    normal.save(os.path.join(OUTPUT_DIR, "T_Tile_Glossy_N.png"))
    print("  法线贴图完成")
    
    # 4. 粗糙度贴图
    roughness = Image.new("L", (SIZE, SIZE))
    r_pixels = roughness.load()
    
    for y in range(SIZE):
        for x in range(SIZE):
            local_x = x % tile_size
            local_y = y % tile_size
            
            in_gap = (local_x < gap_width or local_x > tile_size - gap_width or
                      local_y < gap_width or local_y > tile_size - gap_width)
            
            if in_gap:
                # 填缝处粗糙
                noise = value_noise(x, y, scale=5, seed=456) * 20
                r_pixels[x, y] = clamp(200 + noise)  # ~0.8粗糙度
            else:
                # 瓷砖面光滑（亮面）
                noise = value_noise(x, y, scale=8, seed=789) * 10
                r_pixels[x, y] = clamp(30 + noise)  # ~0.1粗糙度
    
    roughness.save(os.path.join(OUTPUT_DIR, "T_Tile_Glossy_R.png"))
    print("  粗糙度贴图完成")
    
    # 5. 环境光遮蔽贴图
    ao = Image.new("L", (SIZE, SIZE), 255)
    ao_pixels = ao.load()
    
    for y in range(SIZE):
        for x in range(SIZE):
            local_x = x % tile_size
            local_y = y % tile_size
            
            # 计算到最近填缝的距离
            dist_to_gap = min(local_x, local_y, tile_size - local_x, tile_size - local_y)
            
            if dist_to_gap < gap_width * 3:
                # 填缝附近AO降低
                ao_factor = dist_to_gap / (gap_width * 3)
                ao_pixels[x, y] = clamp(150 + ao_factor * 105)
            else:
                ao_pixels[x, y] = 255
    
    ao.save(os.path.join(OUTPUT_DIR, "T_Tile_Glossy_AO.png"))
    print("  AO贴图完成")
    
    print("瓷砖纹理全部完成！")

# ============================================================
# 大理石纹理生成
# ============================================================

def generate_marble_textures():
    """生成抛光大理石全套纹理"""
    print("\n生成大理石纹理...")
    
    # 1. 大理石纹理遮罩（脉络）
    vein_mask = Image.new("L", (SIZE, SIZE))
    vm_pixels = vein_mask.load()
    
    for y in range(SIZE):
        for x in range(SIZE):
            # 使用多层噪声生成大理石脉络
            n1 = fbm(x, y, octaves=6, seed=100)
            n2 = fbm(x + 100, y + 100, octaves=4, seed=200)
            
            # 生成弯曲的脉络
            vein = abs(math.sin((x * 0.01 + n1 * 5) * 1.5 + n2 * 3))
            vein = pow(vein, 3)  # 增强对比度
            
            vm_pixels[x, y] = clamp(vein * 255)
    
    vein_mask.save(os.path.join(OUTPUT_DIR, "T_Marble_Vein_Mask.png"))
    print("  脉络遮罩完成")
    
    # 2. 基础色贴图
    bc = Image.new("RGB", (SIZE, SIZE))
    bc_pixels = bc.load()
    vm_pixels = vein_mask.load()
    
    # 大理石底色（米白色）
    base_color = (235, 230, 220)
    # 脉络颜色（浅灰色）
    vein_color = (180, 175, 165)
    
    for y in range(SIZE):
        for x in range(SIZE):
            vein = vm_pixels[x, y] / 255.0
            
            # 基础噪声
            noise = fbm(x, y, octaves=3, seed=300) * 15 - 7
            
            # 混合底色和脉络色
            r = lerp(base_color[0] + noise, vein_color[0] + noise, vein)
            g = lerp(base_color[1] + noise, vein_color[1] + noise, vein)
            b = lerp(base_color[2] + noise, vein_color[2] + noise, vein)
            
            bc_pixels[x, y] = (clamp(r), clamp(g), clamp(b))
    
    bc.save(os.path.join(OUTPUT_DIR, "T_Marble_Polished_BC.png"))
    print("  基础色贴图完成")
    
    # 3. 高度图
    height = Image.new("L", (SIZE, SIZE))
    h_pixels = height.load()
    
    for y in range(SIZE):
        for x in range(SIZE):
            vein = vm_pixels[x, y] / 255.0
            # 脉络处微凹陷
            noise = fbm(x, y, octaves=4, seed=400) * 15
            h_pixels[x, y] = clamp(220 - vein * 30 + noise)
    
    # 4. 法线贴图
    normal = normal_from_height(height, strength=0.8)
    normal.save(os.path.join(OUTPUT_DIR, "T_Marble_Polished_N.png"))
    print("  法线贴图完成")
    
    # 5. 粗糙度贴图
    roughness = Image.new("L", (SIZE, SIZE))
    r_pixels = roughness.load()
    
    for y in range(SIZE):
        for x in range(SIZE):
            vein = vm_pixels[x, y] / 255.0
            # 抛光面非常光滑，脉络处微粗糙
            noise = fbm(x, y, octaves=3, seed=500) * 8
            base_r = 20  # ~0.08粗糙度
            vein_r = 40  # ~0.15粗糙度
            r_pixels[x, y] = clamp(lerp(base_r, vein_r, vein) + noise)
    
    roughness.save(os.path.join(OUTPUT_DIR, "T_Marble_Polished_R.png"))
    print("  粗糙度贴图完成")
    
    print("大理石纹理全部完成！")

# ============================================================
# 主函数
# ============================================================

if __name__ == "__main__":
    print("=" * 50)
    print("地球Online - 程序纹理生成器")
    print("=" * 50)
    print(f"输出目录: {OUTPUT_DIR}")
    print(f"纹理尺寸: {SIZE}x{SIZE}")
    print()
    
    generate_tile_textures()
    generate_marble_textures()
    
    print("\n" + "=" * 50)
    print("所有纹理生成完成！")
    print("生成的文件：")
    for f in sorted(os.listdir(OUTPUT_DIR)):
        if f.endswith(".png"):
            filepath = os.path.join(OUTPUT_DIR, f)
            size = os.path.getsize(filepath) // 1024
            print(f"  {f} ({size} KB)")
    print("=" * 50)
