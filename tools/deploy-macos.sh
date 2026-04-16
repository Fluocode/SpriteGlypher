#!/usr/bin/env bash
# Bundle Qt into SpriteGlypher.app (same as Release QMAKE_POST_LINK). Add -dmg to also create SpriteGlypher.dmg.
# Usage:
#   ./tools/deploy-macos.sh path/to/SpriteGlypher.app
#   ./tools/deploy-macos.sh path/to/SpriteGlypher.app -dmg
# Optional: set QTDIR to your Qt kit, e.g. export QTDIR="$HOME/Qt/6.11.0/macos"
set -euo pipefail
APP="${1:?Usage: $0 /path/to/SpriteGlypher.app [extra macdeployqt args...]}"
shift || true
QT_BIN="${QTDIR:-}/bin"
if [[ ! -x "${QT_BIN}/macdeployqt" ]]; then
  echo "macdeployqt not found at ${QT_BIN}/macdeployqt" >&2
  echo "Set QTDIR to your Qt macOS install (directory that contains bin/macdeployqt)." >&2
  exit 1
fi
exec "${QT_BIN}/macdeployqt" "$APP" "$@"
