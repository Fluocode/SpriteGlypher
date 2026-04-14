# Sprite Glypher

<p align="center">
  <img src="docs/banner.png" alt="Sprite Glypher — bitmap font tool for games and apps" width="100%" />
</p>

**Sprite Glypher** is a desktop **bitmap font** editor built for **games and applications** where text must look **exactly the same** on every device: you design the look once, bake glyphs into a **texture atlas**, and export layout data your engine or UI layer can consume—without relying on live vector rendering.

---

## Why bitmap fonts?

For many 2D games and stylized apps, pre-rendered glyphs give you **predictable art direction**, **fast drawing** (same cost as sprites), and **consistent output** across platforms. Sprite Glypher helps you go from **TrueType/OpenType** sources to a **PNG atlas** plus a **metrics file** your toolchain already understands.

---

## Highlights

- **Layered effects** — stack fill, stroke, and shadow; work with solid colors, gradients, and image-based fills where supported.
- **Project files** — save your work as `.sgf` and iterate without losing settings.
- **Export** — PNG atlas plus **layout descriptors** (ASCII, XML, binary, JSON) for common game and tool pipelines.
- **Hi-DPI** — optional **@2x** export for sharp assets on high-density screens.

---

## Engine & platform compatibility

Exports are a **PNG atlas** plus **layout data** (ASCII, XML, binary, or JSON). Engines and tools that can read **`.fnt` / XML / binary / JSON together with the texture**—or where you add a small loader—can use these assets.

| Area | Examples |
|------|----------|
| **Flash / Stage3D** | [Starling](https://gamua.com/starling/) — bitmap font + texture. |
| **iOS (legacy)** | [Sparrow](https://github.com/PrimaryFeather/Sparrow-Framework) — bitmap font + atlas (similar pipeline to Starling). |
| **Cocos** | **Cocos2d-x**, **Cocos Creator** — bitmap labels and atlas-based fonts. The original **Cocos2d** (Objective-C) is unmaintained; Creator is the active Cocos product line. |
| **Java / Kotlin** | [LibGDX](https://libgdx.com/) — `BitmapFont` from atlas + descriptor. |
| **C# / .NET** | [MonoGame](https://www.monogame.net/), [FNA](https://github.com/FNA-XNA/FNA) — `SpriteFont`-style content or community loaders for atlas + metrics. |
| **Godot** | Import **bitmap font** (e.g. `.fnt` + image); see [docs](https://docs.godotengine.org/) for your version. |
| **Lua** | [LÖVE](https://love2d.org/) — via libraries that read the exported data; [Defold](https://defold.com/) — bitmap font from `.fnt` + texture. |
| **Haxe** | [Heaps](https://heaps.io/), [OpenFL](https://www.openfl.org/) / HaxeFlixel — bitmap text pipelines. |
| **JavaScript / TypeScript** | [Phaser](https://phaser.io/), [PixiJS](https://pixijs.com/) — bitmap / XML font workflows. |
| **Unity** | Not the default TextMeshPro path; use **Asset Store** or community **importers** that consume the exported XML/JSON + PNG. |
| **Unreal Engine** | Not a built-in UI font format for this workflow; use **plugins** or a **custom importer** for atlas + metrics. |
| **Custom engines** | Any toolkit that parses the descriptor fields and samples the atlas. |

If your engine is not listed, check its documentation for **bitmap fonts**, **`.fnt`**, or **sprite font** import—support is often a built-in bitmap label type or a small importer.

---

<p align="center">
  <img src="docs/icon.png" alt="Sprite Glypher app icon" width="128" height="128" />
</p>

<p align="center">
  <img src="docs/screenshot-main.png" alt="Sprite Glypher main window" width="90%" />
</p>

<p align="center">
  <img src="docs/manylayers.jpg" alt="Sprite Glypher main window" width="90%" />
</p>

<p align="center">
  <img src="docs/shader.jpg" alt="Sprite Glypher main window" width="90%" />
</p>

---

## Download

<p align="center">
Windows release https://github.com/Fluocode/SpriteGlypher/releases/download/release/SpriteGlypher.exe
</p>

<p align="center">
Mac release https://github.com/Fluocode/SpriteGlypher/releases/download/Mac/SpriteGlypher.dmg
</p>

---

Sprite Glypher is an independent open-source project.

***
If You like what I make please donate:
[![Foo](https://www.paypalobjects.com/en_GB/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4QBWVDKEVRL46)
*** 
