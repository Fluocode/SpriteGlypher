#!/usr/bin/env bash
# Package a distributable macOS build into dist/macos:
# - Copies SpriteGlypher.app to dist
# - Runs macdeployqt to bundle Qt frameworks/plugins
# - Optionally creates a DMG
#
# Usage:
#   ./tools/package-macos.sh "/path/to/SpriteGlypher.app"
#   ./tools/package-macos.sh "/path/to/SpriteGlypher.app" --dmg
#
# Optional env:
#   QTDIR="/path/to/Qt/6.x.x/macos"
set -euo pipefail

APP="${1:?Usage: $0 /path/to/SpriteGlypher.app [--dmg]}"
shift || true

MAKE_DMG="0"
for arg in "$@"; do
  if [[ "$arg" == "--dmg" ]]; then
    MAKE_DMG="1"
  fi
done

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DIST_DIR="${ROOT_DIR}/dist/macos/SpriteGlypher"
mkdir -p "${DIST_DIR}"

if [[ ! -d "${APP}" ]]; then
  echo "App not found: ${APP}" >&2
  exit 1
fi

APP_NAME="$(basename "${APP}")"
OUT_APP="${DIST_DIR}/${APP_NAME}"
APP_BASENAME="${APP_NAME%.app}"
DMG_PATH="${DIST_DIR}/${APP_BASENAME}.dmg"

rm -rf "${OUT_APP}"
cp -R "${APP}" "${OUT_APP}"

# macdeployqt won't overwrite an existing qt.conf; remove any prior one
QT_CONF_PATH="${OUT_APP}/Contents/Resources/qt.conf"
if [[ -f "${QT_CONF_PATH}" ]]; then
  rm -f "${QT_CONF_PATH}"
fi

QT_BIN="${QTDIR:-}/bin"
if [[ ! -x "${QT_BIN}/macdeployqt" ]]; then
  echo "macdeployqt not found at ${QT_BIN}/macdeployqt" >&2
  echo "Set QTDIR to your Qt macOS install (directory that contains bin/macdeployqt)." >&2
  exit 1
fi

if [[ "${MAKE_DMG}" == "1" ]]; then
  # hdiutil will fail if the output DMG already exists
  if [[ -f "${DMG_PATH}" ]]; then
    rm -f "${DMG_PATH}"
  fi
  "${QT_BIN}/macdeployqt" "${OUT_APP}" -dmg
else
  "${QT_BIN}/macdeployqt" "${OUT_APP}"
fi

echo ""
echo "Packaged macOS build to:"
echo "  ${OUT_APP}"
if [[ "${MAKE_DMG}" == "1" ]]; then
  echo "DMG should be next to the .app inside dist."
fi

