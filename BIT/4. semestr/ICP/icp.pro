QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blockeditor
TEMPLATE = app
OBJECTS_DIR = src/bin/obj
MOC_DIR = src/bin/moc

QMAKE_CXXFLAGS += -std=c++0x

HEADERS += \
    src/Configuration/ConfigType.h \
    src/Configuration/ConfigPort.h \
    src/Configuration/ConfigTypeMap.h \
    src/Configuration/ConfigBlock.h \
    src/Configuration/ConfigBlockMap.h \
    src/Model/ModelPort.h \
    src/Model/ModelBlock.h \
    src/Model/ModelBlockArray.h \
    src/Model/ModelState.h \
    src/gui/mainwindow.h \
    src/gui/GraphicsBlock.h \
    src/gui/GraphicsConnect.h \
    src/gui/GraphicsPort.h \
    src/gui/Computation.h \
    src/gui/Colors.h

SOURCES += \
    src/gui/main.cpp \
    src/Configuration/ConfigType.cpp \
    src/Configuration/ConfigPort.cpp \
    src/Configuration/ConfigTypeMap.cpp \
    src/Configuration/ConfigBlock.cpp \
    src/Configuration/ConfigBlockMap.cpp \
    src/Model/ModelPort.cpp \
    src/Model/ModelBlock.cpp \
    src/Model/ModelBlockArray.cpp \
    src/Model/ModelState.cpp \
    src/gui/mainwindow.cpp \
    src/gui/GraphicsBlock.cpp \
    src/gui/GraphicsConnect.cpp \
    src/gui/GraphicsPort.cpp \
    src/gui/Computation.cpp \
    src/gui/Colors.cpp

FORMS += \
    src/gui/mainwindow.ui
