#-------------------------------------------------
#
# Project created by QtCreator 2014-03-08T19:19:58
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets xml

TARGET = SpriteGlypher
TEMPLATE = app

macx {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
    QMAKE_INFO_PLIST = "platforms/osx/Info.plist"
    APP_ICON.files = "platforms/osx/spriteglypher.icns"
    APP_ICON.path = Contents/Resources/
    QMAKE_BUNDLE_DATA += APP_ICON
}

ios {
    QMAKE_INFO_PLIST = "platforms/ios/Info.plist"
}

win32 {
    QMAKE_LFLAGS += -static-libgcc

    # Icon RC File
    RC_FILE = "platforms/win/spriteglypher.rc"
}

INCLUDEPATH += $$quote($$PWD)
INCLUDEPATH += "libs/SGWidgets/include/"

# Qt 6 requires C++17 (see QtCore/qcompilerdetection.h).
CONFIG += c++17

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

# Must run after Release:/Debug: set DESTDIR to a single relative segment (e.g. release\), not OUT_PWD/release.
# Use an explicit ".exe": $$TARGET$$TARGET_EXT often expands without the extension here, and windeployqt then skips deployment.
win32 {
    DEPLOY_EXE = $$OUT_PWD/$$DESTDIR/$${TARGET}.exe
    QMAKE_POST_LINK += $$quote($$[QT_INSTALL_BINS]/windeployqt.exe) $$quote($$shell_path($$DEPLOY_EXE)) --compiler-runtime $$escape_expand(\\n\\t)
}

FORMS += \
    forms/MainWindow.ui \
    src/UI/Dialogs/GradientEditorDialog.ui \
    src/UI/Widgets/InputSettingsPanel.ui \
    src/UI/Widgets/GenerationSettingsPanel.ui \
    src/UI/Widgets/FillEffectSettingsPanel.ui \
    src/UI/Widgets/StrokeEffectSettingsPanel.ui \
    src/UI/Widgets/ShadowEffectSettingsPanel.ui \
    src/UI/Widgets/ShadedMaterialEffectSettingsPanel.ui \
    src/UI/Widgets/EffectListRow.ui \
    src/UI/Widgets/ExportSettingsPanel.ui

HEADERS += \
    src/MainWindow.h \
    src/Model/SGFDocument.h \
    src/Model/SGFTypes.h \
    src/Model/SGFGlyph.h \
    src/UI/Widgets/ColorSwatchWidget.h \
    src/Model/Effects/SGFEffect.h \
    src/Model/Effects/SGFFillEffect.h \
    src/Model/Effects/SGFStrokeEffect.h \
    src/Model/Effects/SGFShadowEffect.h \
    src/Model/Effects/SGFShadedMaterialEffect.h \
    src/Model/Effects/SGFEffectTypes.h \
    src/Model/Effects/Effects.h \
    src/UI/Widgets/GradientSwatchWidget.h \
    src/UI/Dialogs/GradientEditorDialog.h \
    src/UI/Widgets/GradientStopEditorWidget.h \
    src/Model/SGFGradient.h \
    src/Model/Exporters/SGFFontExporter.h \
    src/Model/SGFSpriteFont.h \
    src/Model/Exporters/SGFFontBMFontExportData.h \
    src/Model/Exporters/SGFFontExporterBMFont.h \
    src/Model/Exporters/SGFFontExporterBMFontXml.h \
    src/Model/Exporters/SGFFontExporterBMFontBinary.h \
    src/Model/Exporters/SGFFontExporterBMFontJson.h \
    src/Model/Exporters/SGFFontExporterPNG.h \
    src/Model/Exporters/SGFFontExporters.h \
    src/UI/Widgets/InputSettingsPanel.h \
    src/UI/Widgets/GenerationSettingsPanel.h \
    src/UI/Widgets/FillEffectSettingsPanel.h \
    src/UI/Widgets/StrokeEffectSettingsPanel.h \
    src/UI/Widgets/ShadowEffectSettingsPanel.h \
    src/UI/Widgets/ShadedMaterialEffectSettingsPanel.h \
    src/Model/SGFFile/SGFFileWriter.h \
    src/Model/SGFFile/SGFDocumentElement.h \
    src/Model/SGFFile/SGFFileReader.h \
    src/UI/Widgets/PatternSwatchWidget.h \
    src/UI/Widgets/EffectListRow.h \
    src/UI/Widgets/ExportSettingsPanel.h \
    src/UI/Widgets/SGValueSliderRow.h \
    src/UI/Widgets/SGSegmentedControl.h \
    src/Model/SGFExportSettings.h \
    src/Model/Exporters/SGFFontExporterTypes.h \
    src/Model/SGFInputSettings.h \
    src/Model/SGFGenerationSettings.h

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/Model/SGFDocument.cpp \
    src/Model/SGFTypes.cpp \
    src/Model/SGFGlyph.cpp \
    src/UI/Widgets/ColorSwatchWidget.cpp \
    src/Model/Effects/SGFEffect.cpp \
    src/Model/Effects/SGFFillEffect.cpp \
    src/Model/Effects/SGFStrokeEffect.cpp \
    src/Model/Effects/SGFShadowEffect.cpp \
    src/Model/Effects/SGFShadedMaterialEffect.cpp \
    src/UI/Widgets/GradientSwatchWidget.cpp \
    src/UI/Dialogs/GradientEditorDialog.cpp \
    src/UI/Widgets/GradientStopEditorWidget.cpp \
    src/Model/SGFGradient.cpp \
    src/Model/Exporters/SGFFontExporter.cpp \
    src/Model/Exporters/SGFFontBMFontExportData.cpp \
    src/Model/Exporters/SGFFontExporterBMFont.cpp \
    src/Model/Exporters/SGFFontExporterBMFontXml.cpp \
    src/Model/Exporters/SGFFontExporterBMFontBinary.cpp \
    src/Model/Exporters/SGFFontExporterBMFontJson.cpp \
    src/Model/Exporters/SGFFontExporterPNG.cpp \
    src/UI/Widgets/InputSettingsPanel.cpp \
    src/UI/Widgets/GenerationSettingsPanel.cpp \
    src/UI/Widgets/FillEffectSettingsPanel.cpp \
    src/Model/Effects/SGFEffectTypes.cpp \
    src/UI/Widgets/StrokeEffectSettingsPanel.cpp \
    src/UI/Widgets/ShadowEffectSettingsPanel.cpp \
    src/UI/Widgets/ShadedMaterialEffectSettingsPanel.cpp \
    src/Model/SGFFile/SGFFileWriter.cpp \
    src/Model/SGFFile/SGFDocumentElement.cpp \
    src/Model/SGFFile/SGFFileReader.cpp \
    src/UI/Widgets/PatternSwatchWidget.cpp \
    src/UI/Widgets/EffectListRow.cpp \
    src/UI/Widgets/ExportSettingsPanel.cpp \
    src/UI/Widgets/SGValueSliderRow.cpp \
    src/UI/Widgets/SGSegmentedControl.cpp \
    src/Model/SGFExportSettings.cpp \
    src/Model/SGFInputSettings.cpp \
    src/Model/SGFGenerationSettings.cpp

OTHER_FILES += \
    platforms/osx/Info.plist

RESOURCES += \
    resources/resources.qrc

