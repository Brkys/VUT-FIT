TEMPLATE = app
CONFIG += console c++99
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    controller.cpp \
    sniffer.cpp

HEADERS += \
    controller.h \
    sniffer.h

LIBS += -lpcap
