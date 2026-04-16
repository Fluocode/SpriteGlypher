# Building Sprite Glypher

This project is a **Qt Widgets** application. You need the **Qt SDK** (libraries + tools) and a **C++ compiler**. The project uses **C++17** (required by **Qt 6**). It builds with **Qt 5.12+** (e.g. 5.15 LTS) or **Qt 6.x**; on Windows, **MinGW** matches the `.pro` file’s original setup, or use an **MSVC** kit if you prefer.

---

## 1. Install Qt and the toolchain

### Windows (recommended path)

1. **Download the Qt Online Installer**  
   - Go to [https://www.qt.io/download-qt-installer](https://www.qt.io/download-qt-installer) (or use the open-source “Download the Qt Online Installer” link).  
   - Create a Qt account if the installer asks for one.

2. **Run the installer and select components**  
   - Choose an install path without spaces (e.g. `C:\Qt`) to avoid occasional toolchain issues.  
   - Under **Qt**, pick a **Qt 5.15.x** (LTS) build, for example:  
     - **MinGW 8.1.0 64-bit** (recommended; matches common Qt 5.15 kits).  
   - Under **Developer and Designer Tools**, ensure **Qt Creator** and **CMake / Ninja** are optional; for this project you mainly need **Qt Creator** and the **MinGW** toolchain that comes with that Qt version.

3. **Finish the installation** and note your Qt path, e.g. `C:\Qt\5.15.2\mingw81_64`.

You do **not** need a separate MinGW install if you selected the Qt-maintained MinGW kit; Qt Creator will use the compiler bundled with Qt.

### macOS

- Install **Xcode** (or Xcode Command Line Tools) from the App Store / Apple developer site.  
- Install Qt 5.15.x with the **macOS** kit from the same Qt Online Installer.  
- On macOS, `SpriteGlypher.pro` sets `QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15` (required for Qt 6 and `std::filesystem` in libc++). After pulling on Mac, run **Run qmake** then **Rebuild** so the build directory picks up the new flags.

### Linux

- Install Qt 5 development packages for your distribution (names vary by distro), **or** install Qt via the online installer into your home directory.  
- Install `g++`, `make`, and desktop OpenGL/Mesa packages as required by Qt on your distro.

---

## 2. Install Git (if you do not have it)

- **Windows:** [https://git-scm.com/download/win](https://git-scm.com/download/win)  
- Clone the repository:

```bash
git clone https://github.com/YOUR_ORG/spriteglypher.git
cd spriteglypher
```

(Use your real clone URL.)

---

## 3. Open the project in Qt Creator

1. Start **Qt Creator**.  
2. **File → Open File or Project…**  
3. Select **`SpriteGlypher.pro`** in the repo root.  
4. When prompted for a **Kit**, choose the one that matches what you installed, e.g. **Desktop Qt 5.15.x MinGW 64-bit**.  
5. Qt Creator will run **qmake** and load the project.

If no kit appears, open **Tools → Options → Kits** (or **Preferences → Kits** on macOS) and add a kit that points to your Qt installation and compiler.

---

## 4. Build

1. Select the **Release** or **Debug** build configuration (left side of Qt Creator, below the project name).  
2. **Build → Build Project “SpriteGlypher”** (or press `Ctrl+B` / `Cmd+B`).  

Outputs go under a **build** folder next to or under your project (Qt Creator shows the exact path in the **Compile Output** panel).

### macOS — distributable `SpriteGlypher.app`

For **Release** builds, `SpriteGlypher.pro` runs **`macdeployqt`** after linking with **`-dmg`**: it copies Qt frameworks and plugins into **`SpriteGlypher.app`**, then creates **`SpriteGlypher.dmg`** in the same directory.

To re-run deployment without a full rebuild:

```bash
export QTDIR="/path/to/Qt/6.11.0/macos"
./tools/deploy-macos.sh "/path/to/SpriteGlypher.app"
```

---

## 5. Run the `.exe` outside Qt Creator (Windows)

Use `windeployqt` on the Release exe so Qt DLLs and plugins are copied next to it. See `tools/deploy-windows.bat` if you want an example.

### Windows — package into `dist/` (recommended)

This repo includes a PowerShell script that creates a clean distributable folder:

```powershell
.\tools\package-windows.ps1 -ExePath "C:\path\to\SpriteGlypher.exe"
```

It will output to `dist/windows/SpriteGlypher/` (exe + Qt DLLs + plugins).

# Building Sprite Glypher

This project is a **Qt Widgets** application. You need the **Qt SDK** (libraries + tools) and a **C++ compiler**. The project uses **C++17** (required by **Qt 6**). It builds with **Qt 5.12+** (e.g. 5.15 LTS) or **Qt 6.x**; on Windows, **MinGW** matches the `.pro` file’s original setup, or use an **MSVC** kit if you prefer.

---

## 1. Install Qt and the toolchain

### Windows (recommended path)

1. **Download the Qt Online Installer**  
   - Go to [https://www.qt.io/download-qt-installer](https://www.qt.io/download-qt-installer) (or use the open-source “Download the Qt Online Installer” link).  
   - Create a Qt account if the installer asks for one.

2. **Run the installer and select components**  
   - Choose an install path without spaces (e.g. `C:\Qt`) to avoid occasional toolchain issues.  
   - Under **Qt**, pick a **Qt 5.15.x** (LTS) build, for example:  
     - **MinGW 8.1.0 64-bit** (recommended; matches common Qt 5.15 kits).  
   - Under **Developer and Designer Tools**, ensure **Qt Creator** and **CMake / Ninja** are optional; for this project you mainly need **Qt Creator** and the **MinGW** toolchain that comes with that Qt version.

3. **Finish the installation** and note your Qt path, e.g. `C:\Qt\5.15.2\mingw81_64`.

You do **not** need a separate MinGW install if you selected the Qt-maintained MinGW kit; Qt Creator will use the compiler bundled with Qt.

### macOS

- Install **Xcode** (or Xcode Command Line Tools) from the App Store / Apple developer site.  
- Install Qt 5.15.x with the **macOS** kit from the same Qt Online Installer.  
- The `.pro` file sets `QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8` and uses `libc++`; use a Qt kit that matches your macOS version.

### Linux

- Install Qt 5 development packages for your distribution (e.g. `qt5-default`, `qtmultimedia5`, `libqt5widgets5`, etc., names vary by distro), **or** install Qt via the online installer into your home directory.  
- Install `g++`, `make`, and desktop OpenGL/Mesa packages as required by Qt on your distro.

---

## 2. Install Git (if you do not have it)

- **Windows:** [https://git-scm.com/download/win](https://git-scm.com/download/win)  
- Clone the repository:

```bash
git clone https://github.com/YOUR_ORG/spriteglypher.git
cd spriteglypher
```

(Use your real clone URL.)

---

## 3. Open the project in Qt Creator

1. Start **Qt Creator**.  
2. **File → Open File or Project…**  
3. Select **`SpriteGlypher.pro`** in the repo root.  
4. When prompted for a **Kit**, choose the one that matches what you installed, e.g. **Desktop Qt 5.15.x MinGW 64-bit**.  
5. Qt Creator will run **qmake** and load the project.

If no kit appears, open **Tools → Options → Kits** (or **Preferences → Kits** on macOS) and add a kit that points to your Qt installation and compiler.

---

## 4. Build

1. Select the **Release** or **Debug** build configuration (left side of Qt Creator, below the project name).  
2. **Build → Build Project “SpriteGlypher”** (or press `Ctrl+B` / `Cmd+B`).  

Outputs go under a **build** folder next to or under your project (Qt Creator shows the exact path in the **Compile Output** panel). Typical layout:

- `build-.../release/SpriteGlypher.exe` (Windows Release)  
- or `release/SpriteGlypher` in the shadow build directory.

---

## 5. Run the application from Qt Creator

**Build → Run** (or the green Run button). Qt Creator sets `PATH` so the correct Qt DLLs are found.

---

## 6. Run the `.exe` outside Qt Creator (Windows)

Release builds may not find Qt DLLs when you double-click the exe from Explorer. Two reliable options:

### Option A — `windeployqt` (recommended)

Open a shell where Qt’s `bin` is on `PATH`, then:

```bat
cd C:\path\to\your\build\release
"C:\Qt\5.15.2\mingw81_64\bin\windeployqt.exe" SpriteGlypher.exe
```

Adjust the Qt path to match your install. This copies the needed Qt plugins and DLLs next to the executable.

### Option B — Copy DLLs manually

The `SpriteGlypher.pro` file contains a **Windows Release** section that copies some Qt DLLs and platform plugins via `QMAKE_POST_LINK`. That list was written for an older Qt 5 layout (including ICU DLL names like `icudt51.dll`). On a **newer** Qt 5.15 install, ICU names and optional dependencies differ, so **`windeployqt` is simpler** than maintaining that list by hand.

---

## 7. Optional: Designer plugin (`SGWidgets`)

- **Only needed** if you want custom widgets inside **Qt Designer**.  
- Open **`libs/SGWidgets/SGWidgets.pro`** in Qt Creator and build the **Designer** target; see comments in the repo `readme.md`.  
- **Not required** to compile and run the main **Sprite Glypher** app (widget sources are included in the main project).

---

## 8. Troubleshooting

| Problem | What to try |
|--------|-------------|
| **“Qt requires a C++17 compiler”** | The project is configured with **`CONFIG += c++17`** in `SpriteGlypher.pro`. Run **Build → Rebuild** after pulling that change. If it still fails, your kit may be using an old compiler; install a newer **MinGW** (bundled with Qt 6 / Qt 5.15) or MSVC that supports C++17. |
| **“No valid settings file” / incompatible `.pro.user`** | That file is **local IDE state** (kit paths, old Qt Creator version). Close Qt Creator, delete **`SpriteGlypher.pro.user`** and the **`.qtcreator`** folder in the project if present, then open **`SpriteGlypher.pro`** again. Qt Creator will create a fresh settings file. These files should **not** be committed to Git. |
| “No valid kit” | Install a **Desktop** Qt kit with a compiler; configure **Kits** in Qt Creator. |
| `qmake` not found | Open project with Qt Creator so it uses the Qt version you installed; or add `...\Qt\5.15.x\mingw81_64\bin` to `PATH`. |
| App starts in Qt Creator but not from Explorer | Run **`windeployqt`** on the Release `exe`, or run from the **build** directory with DLLs copied as above. |
| Link errors about Qt modules | Ensure **`SpriteGlypher.pro`** lists `core`, `gui`, `widgets`, `xml`, `concurrent` (already there); reinstall Qt with the **MinGW** / **MSVC** kit you selected for the project. |

---

## 9. Summary checklist

1. Install **Qt 5.15.x** + **MinGW** (Windows) or the appropriate kit (macOS/Linux).  
2. Install **Qt Creator**.  
3. Open **`SpriteGlypher.pro`**.  
4. Pick a matching **Kit**, **Build**, **Run**.  
5. For a portable folder on Windows, run **`windeployqt`** on the built `SpriteGlypher.exe`.

After that, you can iterate on the code and use **Build → Rebuild** whenever you change project files or dependencies.
