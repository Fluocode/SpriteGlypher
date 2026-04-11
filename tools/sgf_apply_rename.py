#!/usr/bin/env python3
"""
SpriteFontBuilder -> SpriteGlypher, SFB* -> SGF* (except SGWidgets), .sfb -> .sgf
Run: python tools/sgf_apply_rename.py
"""
from __future__ import annotations

import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

SKIP_DIR_PARTS = {".git", "build", "SGWidgets"}

TEXT_SUFFIXES = (
    ".cpp",
    ".h",
    ".ui",
    ".pro",
    ".qrc",
    ".rc",
    ".plist",
    ".nsi",
    ".bat",
    ".md",
    ".pri",
    ".rtf",
)


def should_skip_dir(dp: str) -> bool:
    parts = dp.replace("\\", "/").split("/")
    return any(p in SKIP_DIR_PARTS for p in parts)


def collect_sfb_tokens(text: str) -> set[str]:
    return set(re.findall(r"\bSFB[A-Za-z0-9_]*", text))


def build_replacements() -> list[tuple[str, str]]:
    tokens: set[str] = set()
    for dp, dns, fns in os.walk(ROOT):
        if should_skip_dir(dp):
            dns[:] = []
            continue
        for fn in fns:
            if not any(fn.endswith(s) for s in TEXT_SUFFIXES):
                continue
            path = os.path.join(dp, fn)
            try:
                data = open(path, encoding="utf-8", errors="replace").read()
            except OSError:
                continue
            tokens |= collect_sfb_tokens(data)

    tokens.discard("SGWidgets")
    # Bare "SFB" would substring-replace inside "SGWidgets" -> "SGFWidgets" if that token were missing.
    tokens.discard("SFB")

    pairs = [(t, "SGF" + t[3:]) for t in tokens]
    pairs.sort(key=lambda ab: (-len(ab[0]), ab[0]))
    return pairs


def transform_text(data: str, pairs: list[tuple[str, str]]) -> str:
    for old, new in pairs:
        data = data.replace(old, new)
    # k-prefixed keys (not matched by \bSFB...)
    data = re.sub(r"\bkSFB([A-Za-z0-9_]*)", r"kSGF\1", data)
    data = data.replace("SpriteFontBuilder", "SpriteGlypher")
    data = data.replace("spritefontbuilder", "spriteglypher")
    data = data.replace("Sprite Font Builder", "Sprite Glypher")
    data = data.replace(".sfbx", ".sgfx")
    data = data.replace(".sfb", ".sgf")
    # Shared widget library path must stay SGWidgets (bare "SFB" token would have mangled it).
    data = data.replace("libs/SGFWidgets", "libs/SGWidgets")
    return data


def process_all_text(pairs: list[tuple[str, str]]) -> None:
    for dp, dns, fns in os.walk(ROOT):
        if should_skip_dir(dp):
            dns[:] = []
            continue
        for fn in fns:
            if not any(fn.endswith(s) for s in TEXT_SUFFIXES):
                continue
            path = os.path.join(dp, fn)
            try:
                orig = open(path, encoding="utf-8", errors="replace").read()
            except OSError:
                continue
            new = transform_text(orig, pairs)
            if new != orig:
                open(path, "w", encoding="utf-8", newline="").write(new)


def rename_sfb_sources() -> None:
    moves: list[tuple[str, str]] = []
    for dp, dns, fns in os.walk(ROOT, topdown=False):
        if should_skip_dir(dp):
            continue
        for fn in fns:
            if not (fn.startswith("SFB") and fn.endswith((".h", ".cpp"))):
                continue
            oldp = os.path.join(dp, fn)
            newp = os.path.join(dp, "SGF" + fn[3:])
            moves.append((oldp, newp))
        if os.path.basename(dp) == "SFBFile":
            parent = os.path.dirname(dp)
            moves.append((dp, os.path.join(parent, "SGFFile")))

    moves.sort(key=lambda ab: (-len(ab[0]), ab[0]))
    for oldp, newp in moves:
        if not os.path.exists(oldp):
            continue
        if os.path.exists(newp):
            continue
        os.makedirs(os.path.dirname(newp), exist_ok=True)
        os.rename(oldp, newp)


def rename_branding_assets() -> None:
    mapping = [
        ("platforms/win/spritefontbuilder.rc", "platforms/win/spriteglypher.rc"),
        ("platforms/win/spritefontbuilder.ico", "platforms/win/spriteglypher.ico"),
        ("platforms/osx/spritefontbuilder.icns", "platforms/osx/spriteglypher.icns"),
        ("platforms/win/spritefontbuilder.nsi", "platforms/win/spriteglypher.nsi"),
    ]
    for rel_old, rel_new in mapping:
        oldp = os.path.join(ROOT, rel_old.replace("/", os.sep))
        newp = os.path.join(ROOT, rel_new.replace("/", os.sep))
        if os.path.isfile(oldp) and not os.path.exists(newp):
            os.rename(oldp, newp)


def rename_project_file() -> None:
    old = os.path.join(ROOT, "SpriteFontBuilder.pro")
    new = os.path.join(ROOT, "SpriteGlypher.pro")
    if os.path.isfile(old) and not os.path.exists(new):
        os.rename(old, new)


def main() -> int:
    os.chdir(ROOT)
    pairs = build_replacements()
    process_all_text(pairs)
    rename_sfb_sources()
    rename_branding_assets()
    rename_project_file()
    print("OK: SGF rename applied.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
