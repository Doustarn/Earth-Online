# Immortal Dao Online

A large-scale open-world **Xianxia (cultivation) MMORPG** built on **Unreal Engine 5.8**.
No fixed storyline, no forced quests — you start as a random cultivator and rise through 21 major realms toward transcendence.

> Developed by **Doustar / Hackm** · Current version **v0.1.8.1.1001** (Alpha)

---

## ✨ Features

- **21 Major Realms × 10 sub-levels each** — Qi Refining → Foundation → Core Formation → Nascent Soul → Soul Transformation → Body Integration → Tribulation → Mahayana → Immortal Tribulation → Loose Immortal → Earth Immortal → Heavenly Immortal → Golden Immortal → Taiyi Golden → Daluo Golden → Quasi-Sage → Sub-Saint → Sage → Eternal Sage → Dao Heavenly → Transcendent → Beyond
- **Random NPC inheritance start** — the world runs entirely on NPCs; on registration you randomly inherit an NPC identity (sect + Dao name + realm), never above Soul Transformation
- **5 Playable Classes** — Sword / Spear / Mage / Alchemist / Talisman, each with its own skill tree
- **Sect system** — at Core Formation you may found your own sect: main hall, scripture pavilion, alchemy hall, grand protective array; recruit NPC disciples, ally or declare war on other sects over spirit mines
- **Tiered cultivation currency** — Spirit Stones (4 tiers) → Divine Stones → Immortal Crystals → Dao Stones → Merit → Fortune → Dao Proof
- **Offline decay** — after 1 year of inactivity: low realms (Qi ~ Transformation) drop 3 major realms, high realms (Integration+) drop 2; floor at Qi Refining
- **Bilingual** — auto-detected by IP on registration: Simplified Chinese for CN regions, English elsewhere; switchable in-game
- **Open-world grinding** — Bandits, demon beasts and rogue cultivators roam the wilds, dropping cultivation EXP and spirit stones

## 🛠 Tech Stack

| Layer | Choice |
|-------|--------|
| Engine | Unreal Engine 5.8.1 (DX12, SM6) |
| Language | C++ + UMG (dynamically constructed UI) |
| Network | UE built-in replication (NetServerMaxTickRate = 60) |
| i18n | Static language manager + on-registration selection |

## 📂 Repository Structure

```
Immortal-Dao-Online/
├── UE5/
│   ├── Config/            # Engine / Input / Game config
│   ├── Content/           # Maps, UI textures, materials
│   ├── Source/EarthOnline/
│   │   ├── Character/         # Player character
│   │   ├── GameInstance/      # Network / server
│   │   ├── GameMode/          # Game mode (enemy spawn + decay)
│   │   ├── HUD/               # UI flow
│   │   ├── PlayerController/  # Input / interaction
│   │   ├── Utils/             # i18n EOLang
│   │   └── UI/                # Main menu / login / HUD / class select
│   └── EarthOnline.uproject
└── LICENSE                # AGPLv3
```

## 🚀 Getting Started

1. Install Unreal Engine **5.8.1**
2. Clone this repo
3. Right-click `EarthOnline.uproject` → **Generate Visual Studio project files**
4. Open the `.sln`, build **Development Editor**, hit Play

## 📦 Releases

Prebuilt Windows clients are published on the
[Releases](https://github.com/Doustarn/Immortal-Dao-Online/releases) page.

> **Hackm note**: This is an early alpha — bugs are expected and feedback is welcome!

## 📡 Mirrors

| Platform | URL |
|----------|-----|
| GitHub | https://github.com/Doustarn/Immortal-Dao-Online |
| Gitee  | https://gitee.com/Doustar/Immortal-Dao-Online |

## 📄 License

**Immortal Dao Online** is free software: you can redistribute it and/or modify
it under the terms of the **GNU Affero General Public License v3.0** as
published by the Free Software Foundation.

See [LICENSE](./LICENSE) for the full text.
