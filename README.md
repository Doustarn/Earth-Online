# Earth Online

A high-fidelity open-world sandbox MMORPG built on **Unreal Engine 5.8**.
No preset storyline, no rail-roaded campaign — every player choice shapes their own life on the planet, and death is permanent.

> Developed by **Doustar / 逗星科技** · Current version **v0.1.8.1.1001** (closed beta)

---

## ✨ Features

- **Open-world sandbox** — free-roam life simulation, no forced questline
- **UGC building system** — place / move / rotate / scale boxes, spheres, planes, cylinders, cones; save and load custom maps
- **Full input support** — WASD + mouse + gamepad; `~`-key mouse lock (Black Myth: Wukong style)
- **Polished UI**
  - Main menu: rotating Earth globe over a real starfield backdrop
  - Login / loading screen: 4-stage loading animation with flash transitions
  - In-game HUD: health / stamina / minimap / 9-slot hotbar / interaction prompts / new-player tutorial / alerts
  - In-game mobile phone: home screen + settings (sensitivity / resolution / quality / volume)
- **Beta key system** — SHA-256 hashed activation keys
- **Next-gen rendering** — Nanite, Lumen, Virtual Shadow Maps, hardware ray tracing, Substrate

## 🛠 Tech Stack

| Layer | Choice |
|-------|--------|
| Engine | Unreal Engine 5.8.1 (DX12, SM6) |
| Language | C++ + UMG (dynamic UI construction) |
| Network | UE built-in replication (NetServerMaxTickRate = 60) |
| Auth | SHA-256 beta key activation |

## 📂 Repository Structure

```
Earth-online/
├── UE5/
│   ├── Config/            # Engine / Input / Game config
│   ├── Content/           # Maps, UI textures, materials
│   ├── Source/EarthOnline/
│   │   ├── Character/         # Player character
│   │   ├── GameInstance/      # Network / server
│   │   ├── GameMode/          # Game mode
│   │   ├── HUD/               # UI flow
│   │   ├── PlayerController/  # Input / interaction
│   │   ├── Security/          # Beta key manager
│   │   ├── UGC/               # Building system
│   │   └── UI/                # Main menu / login / HUD / phone
│   └── EarthOnline.uproject
├── 开发日志.txt
└── LICENSE                # AGPLv3
```

## 🚀 Getting Started

1. Install Unreal Engine **5.8.1** (from Epic Games Launcher).
2. Clone this repo.
3. Right-click `EarthOnline.uproject` → **Generate Visual Studio project files**.
4. Open the `.sln`, build the **Development Editor** config, and hit Play.

## 📦 Releases

Prebuilt Windows clients are published on the
[Releases](https://github.com/Doustarn/Earth-Online/releases) page,
each tagged with a beta activation key.

## 📡 Mirrors

| Platform | URL |
|----------|-----|
| GitHub | https://github.com/Doustarn/Earth-Online |
| Gitee  | https://gitee.com/Doustar/Earth-online |

## 📄 License

**Earth Online** is free software: you can redistribute it and/or modify
it under the terms of the **GNU Affero General Public License v3.0** as
published by the Free Software Foundation.

See [LICENSE](./LICENSE) for the full text.
