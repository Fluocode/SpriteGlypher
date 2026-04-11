CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(SGWidgets)
TEMPLATE    = lib

QT += widgets

macx {
    #QMAKE_CXXFLAGS += -stdlib=libc++
    #QMAKE_LFLAGS += -stdlib=libc++
    #QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    #CONFIG += c++11
}

RESOURCES   = resources/icons.qrc
LIBS        += -L. 

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

DEFINES += SGWIDGETS_LIBRARY
INCLUDEPATH += include/

Release:DESTDIR = $$quote($$PWD/release/shared)
Release:OBJECTS_DIR = release/obj
Release:MOC_DIR = release/moc
Release:RCC_DIR = release/rcc
Release:UI_DIR = release/ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/obj
Debug:MOC_DIR = debug/moc
Debug:RCC_DIR = debug/rcc
Debug:UI_DIR = debug/ui

count(STATIC, 1) {
    CONFIG      += static

    Release:DESTDIR = $$quote($$PWD/release/static)
    Release:OBJECTS_DIR = static/.obj
    Release:MOC_DIR = static/.moc
    Release:RCC_DIR = static/.rcc
    Release:UI_DIR = static/.ui
}

include(Widgets.pri)
include(Designer.pri)

FORMS += \
    testing/TestDialoig.ui

OTHER_FILES += \
    resources/SGWidgets.json
