QT += core
QT += widgets
QT += printsupport
QT += charts
QT += gui

CONFIG += c++11

TARGET = menuVis
CONFIG += app_bundle

TEMPLATE = app

SOURCES += src/*.cpp
HEADERS += inc/*.h
FORMS += ui/*.ui
RESOURCES += res/*.qrc

include(ext/qtxlsx/src/xlsx/qtxlsx.pri)

INCLUDEPATH += inc/

CONFIG(debug,debug|release) DESTDIR = build/debug
CONFIG(release,debug|release) DESTDIR = build/release
MOC_DIR = $$DESTDIR/moc
OBJECTS_DIR = $$DESTDIR/obj
RCC_DIR = $$DESTDIR/rcc
UI_DIR = $$DESTDIR/uci

DEFINES += QT_DEPRECATED_WARNINGS
